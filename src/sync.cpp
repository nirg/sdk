/*

MEGA SDK - Client Access Engine Core Logic

(c) 2013 by Mega Limited, Wellsford, New Zealand

Author: mo
Bugfixing: js, mr

Applications using the MEGA API must present a valid application key
and comply with the the rules set forth in the Terms of Service.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include "mega/sync.h"
#include "mega/megaapp.h"
#include "mega/transfer.h"

namespace mega {

// a new sync reads the full local tree and issues all commands required to equalize both sides
Sync::Sync(MegaClient* cclient, string* crootpath, Node* remotenode, int ctag)
{
	client = cclient;
	tag = ctag;

	localbytes = 0;
	localnodes[FILENODE] = 0;
	localnodes[FOLDERNODE] = 0;

	state = SYNC_INITIALSCAN;
	localroot.init(this,crootpath,FOLDERNODE,NULL,crootpath);
	localroot.setnode(remotenode);

	queuescan(NULL,NULL,NULL,NULL,true);
	procscanq();

	sync_it = client->syncs.insert(client->syncs.end(),this);
}

Sync::~Sync()
{
	// prevent mass deletion while rootlocal destructor runs
	state = SYNC_CANCELED;

	client->syncs.erase(sync_it);

	client->syncactivity = true;
}

void Sync::changestate(syncstate newstate)
{
	if (newstate != state)
	{
		client->app->syncupdate_state(this,newstate);

		state = newstate;
	}
}

// walk path and return state of the sync
// path must not start with a separator and be relative to the sync root
pathstate_t Sync::pathstate(string* localpath)
{
	const char* ptr = localpath->data();
	const char* end = localpath->data()+localpath->size();
	const char* nptr = ptr;
	LocalNode* l = &localroot;
	size_t separatorlen = client->fsaccess->localseparator.size();
	localnode_map::iterator it;
	string t;

	for (;;)
	{
		if (nptr == end || !memcmp(nptr,client->fsaccess->localseparator.data(),separatorlen))
		{
			t.assign(ptr,nptr-ptr);

			if ((it = l->children.find(&t)) == l->children.end()) return PATHSTATE_NOTFOUND;

			l = it->second;

			if (nptr == end) break;

			ptr = nptr+separatorlen;
			nptr = ptr;
		}
		else nptr += separatorlen;
	}

	if (l->node) return PATHSTATE_SYNCED;
	if (l->transfer && l->transfer->slot) return PATHSTATE_SYNCING;
	return PATHSTATE_PENDING;
}


// scan rootpath, add or update child nodes, call recursively for folder nodes
void Sync::scan(string* localpath, FileAccess* fa, LocalNode* parent, bool fulltree)
{
	DirAccess* da;
	string localname;
	static handle scanseqno;
	localnode_map::iterator it;
	LocalNode* l;

	scanseqno++;

	da = client->fsaccess->newdiraccess();

	// scan the dir, mark all items with a unique identifier
	if (da->dopen(localpath,fa,false)) while (da->dnext(&localname)) if ((l = queuefsrecord(localpath,&localname,parent,fulltree))) l->scanseqno = scanseqno;

	// delete items that disappeared
	for (it = parent->children.begin(); it != parent->children.end(); )
	{
		if (scanseqno != it->second->scanseqno) delete (it++)->second;
		else it++;
	}

	delete da;
}

LocalNode* Sync::queuefsrecord(string* localpath, string* localname, LocalNode* parent, bool fulltree)
{
	localnode_map::iterator it;
	LocalNode* l;

	// check if this record is to be ignored
	if (!client->fsaccess->localhidden(localpath,localname))
	{
		l = (it = parent->children.find(localname)) != parent->children.end() ? it->second : NULL;
		queuescan(localpath,localname,l,parent,fulltree);

		return l;
	}

	return NULL;
}

void Sync::queuescan(string* localpath, string* localname, LocalNode* localnode, LocalNode* parent, bool fulltree)
{
	// FIXME: efficient copy-free push_back? C++11 emplace()?
	scanq.resize(scanq.size()+1);

	ScanItem* si = &scanq.back();

	// FIXME: don't create mass copies of localpath
	if (localpath) si->localpath = *localpath;
	if (localname) si->localname = *localname;
	si->localnode = localnode;
	si->parent = parent;
	si->fulltree = fulltree;
	si->deleted = false;
}

// add or refresh local filesystem item from scan stack, add items to scan stack
// must be called with a scanq.siz() > 0
void Sync::procscanq()
{
	ScanItem* si = &*scanq.begin();

	// ignore deleted ScanItems
	if (si->deleted)
	{
		scanq.pop_front();
		return;
	}

	string* localpath = &si->localpath;
	string* localname = &si->localname;

	bool fulltree = si->fulltree;

	FileAccess* fa;
	bool changed = false;

	string tmpname;

	LocalNode* l;

	// if localpath was not specified, construct based on parents & base sync path
	if (!localpath->size())
	{
		LocalNode* p = si->parent ? si->parent : &localroot;

		while (p)
		{
			localpath->insert(0,p->localname);
			if ((p = p->parent)) localpath->insert(0,client->fsaccess->localseparator);
		}
	}

	if (localname->size())
	{
		localpath->append(client->fsaccess->localseparator);
		localpath->append(*localname);
	}

	// check if a child by the same name already exists
	// (skip this check for localroot)
	if (si->parent)
	{
		// have we seen this item before?
		localnode_map::iterator it = si->parent->children.find(localname);

		if (it != si->parent->children.end()) l = it->second;
		else l = NULL;
	}
	else l = NULL;

	if (l)
	{
		localnodes[l->type]--;
		if (l->type == FILENODE) localbytes -= l->size;
	}

	// attempt to open/type this file, bail if unsuccessful
	fa = client->fsaccess->newfileaccess();

	if (fa->fopen(localpath,1,0))
	{
		if (si->parent)
		{
			if (l && l->type != fa->type)
			{
				// type change (a directory replaced a file of the same name or vice versa)
				delete l;
				l = NULL;
			}

			// new node
			if (!l)
			{
				// this is a new node: add
				l = new LocalNode;
				l->init(this,localname,fa->type,si->parent,localpath);

				changed = true;
			}
		}

		// detect file changes or recurse into new subfolders
		if (fa->type == FOLDERNODE)
		{
			if (fulltree) scan(localpath,fa,si->parent ? l : &localroot,fulltree);
		}
		else
		{
			if (!l) changestate(SYNC_FAILED);	// root node cannot be a file
			else if (l->genfingerprint(fa)) changed = true;
		}

		if (changed) client->syncadded.insert(l->syncid);
	}
	else
	{
		if (fa->retry)
		{
			// fopen() signals that the failure is potentially transient - do nothing, but request a recheck

		}
		else if (l)
		{
			// file gone
			client->fsaccess->local2path(localpath,&tmpname);
			if (l->type == FOLDERNODE) client->app->syncupdate_local_folder_deletion(this,tmpname.c_str());
			else client->app->syncupdate_local_file_deletion(this,tmpname.c_str());

			client->syncactivity = true;

			delete l;
			l = NULL;
		}
	}

	if (l)
	{
		if (changed)
		{
			client->syncactivity = true;
			client->fsaccess->local2path(localpath,&tmpname);
		}

		localnodes[l->type]++;

		if (l->type == FILENODE)
		{
			if (changed) client->app->syncupdate_local_file_addition(this,tmpname.c_str());
			localbytes += l->size;
		}
		else if (changed) client->app->syncupdate_local_folder_addition(this,tmpname.c_str());
	}

	delete fa;

	scanq.pop_front();

	if (scanq.size()) client->syncactivity = true;
}

} // namespace
