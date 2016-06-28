/**
 * @file megacmd.h
 * @brief sample application, interactive GNU Readline CLI
 *
 * (c) 2013 by Mega Limited, Wellsford, New Zealand
 *
 * This file is part of the MEGA SDK - Client Access Engine.
 *
 * Applications using the MEGA API must present a valid application key
 * and comply with the the rules set forth in the Terms of Service.
 *
 * The MEGA SDK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * @copyright Simplified (2-clause) BSD License.
 *
 * You should have received a copy of the license along with this
 * program.
 */

#include "megaapi.h"

using namespace mega;


//extern MegaClient* client;
//extern MegaApi* api;


extern void megacmd();

extern void term_init();
extern void term_restore();
extern void term_echo(int);

extern void read_pw_char(char*, int, int*, char**);

//typedef list<struct AppFile*> appfile_list;

//class MegaCmdListener : public MegaRequestListener
//{
//public:
//    MegaCmdListener(MegaApi *megaApi, MegaRequestListener *listener = NULL);
//    virtual ~MegaCmdListener();

//    //Request callbacks
//    virtual void onRequestStart(MegaApi* api, MegaRequest *request);
//    virtual void onRequestFinish(MegaApi* api, MegaRequest *request, MegaError* e);
//    virtual void onRequestUpdate(MegaApi* api, MegaRequest *request);
//    virtual void onRequestTemporaryError(MegaApi *api, MegaRequest *request, MegaError* e);

//protected:
//    //virtual void customEvent(QEvent * event);

//    MegaRequestListener *listener;
//    MegaApi *megaApi;
//};


/*struct DemoApp : public MegaApp //TODO: consider deleting
{
    FileAccess* newfile();

    void request_error(error);

    void request_response_progress(m_off_t, m_off_t);

    void login_result(error);

    void ephemeral_result(error);
    void ephemeral_result(handle, const byte*);

    void sendsignuplink_result(error);
    void querysignuplink_result(error);
    void querysignuplink_result(handle, const char*, const char*, const byte*, const byte*, const byte*, size_t);
    void confirmsignuplink_result(error);
    void setkeypair_result(error);

    void users_updated(User**, int);
    void nodes_updated(Node**, int);
    void pcrs_updated(PendingContactRequest**, int);
    void nodes_current();

#ifdef ENABLE_CHAT
    void chatcreate_result(TextChat *, error);
    void chatfetch_result(textchat_vector *chats, error);
    void chatinvite_result(error);
    void chatremove_result(error);
    void chaturl_result(string *, error);
    void chatgrantaccess_result(error);
    void chatremoveaccess_result(error);

    void chats_updated(textchat_vector *);

    void printChatInformation(TextChat *);
    string getPrivilegeString(privilege_t priv);
#endif

    int prepare_download(Node*);

    void setattr_result(handle, error);
    void rename_result(handle, error);
    void unlink_result(handle, error);

    void fetchnodes_result(error);

    void putnodes_result(error, targettype_t, NewNode*);

    void share_result(error);
    void share_result(int, error);

    void setpcr_result(handle, error, opcactions_t);
    void updatepcr_result(error, ipcactions_t);

    void fa_complete(Node*, fatype, const char*, uint32_t);
    int fa_failed(handle, fatype, int, error);

    void putfa_result(handle, fatype, error);

    void invite_result(error);
    void putua_result(error);
    void getua_result(error);
    void getua_result(byte*, unsigned);

    void account_details(AccountDetails*, bool, bool, bool, bool, bool, bool);
    void account_details(AccountDetails*, error);

    // sessionid is undef if all sessions except the current were killed
    void sessions_killed(handle sessionid, error e);

    void exportnode_result(error);
    void exportnode_result(handle, handle);

    void openfilelink_result(error);
    void openfilelink_result(handle, const byte*, m_off_t, string*, string*, int);

    void checkfile_result(handle, error);
    void checkfile_result(handle, error, byte*, m_off_t, m_time_t, m_time_t, string*, string*, string*);

    dstime pread_failure(error, int, void*);
    bool pread_data(byte*, m_off_t, m_off_t, void*);

    void transfer_added(Transfer*);
    void transfer_removed(Transfer*);
    void transfer_prepare(Transfer*);
    void transfer_failed(Transfer*, error);
    void transfer_update(Transfer*);
    void transfer_limit(Transfer*);
    void transfer_complete(Transfer*);

#ifdef ENABLE_SYNC
    void syncupdate_state(Sync*, syncstate_t);
    void syncupdate_scanning(bool);
    void syncupdate_local_folder_addition(Sync*, LocalNode*, const char*);
    void syncupdate_local_folder_deletion(Sync* , LocalNode*);
    void syncupdate_local_file_addition(Sync*, LocalNode*, const char*);
    void syncupdate_local_file_deletion(Sync*, LocalNode*);
    void syncupdate_local_file_change(Sync*, LocalNode*, const char*);
    void syncupdate_local_move(Sync*, LocalNode*, const char*);
    void syncupdate_local_lockretry(bool);
    void syncupdate_get(Sync*, Node*, const char*);
    void syncupdate_put(Sync*, LocalNode*, const char*);
    void syncupdate_remote_file_addition(Sync*, Node*);
    void syncupdate_remote_file_deletion(Sync*, Node*);
    void syncupdate_remote_folder_addition(Sync*, Node*);
    void syncupdate_remote_folder_deletion(Sync*, Node*);
    void syncupdate_remote_copy(Sync*, const char*);
    void syncupdate_remote_move(Sync*, Node*, Node*);
    void syncupdate_remote_rename(Sync*, Node*, const char*);
    void syncupdate_treestate(LocalNode*);

    bool sync_syncable(Node*);
    bool sync_syncable(const char*, string*, string*);
#endif

    void changepw_result(error);

    void userattr_update(User*, int, const char*);

    void enumeratequotaitems_result(handle, unsigned, unsigned, unsigned, unsigned, unsigned, const char*);
    void enumeratequotaitems_result(error);
    void additem_result(error);
    void checkout_result(error);
    void checkout_result(const char*);

    void reload(const char*);
    void clearing();

    void notify_retry(dstime);
};*/