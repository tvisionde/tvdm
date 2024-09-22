#include "pam.h"

static void init_env(struct passwd* pw);
static void set_env(char* name, char* value);
static int end(int last_result);

bool login(const char* username, const char* password, pid_t* child_pid)
{
    const char* data[2] = {username , password};
    struct pam_conv pamConv = {
        conv, data
    };
    
    int result = pam_start(SERVICE_NAME, username, &pamConv, &pam_handle);
    if (result != PAM_SUCCESS) {
        error("pam_start failed.", result);
        return false;
    }

    result = pam_authenticate(pam_handle, 0);
    if (result != PAM_SUCCESS) {
        error("pam_authenticate failed.", result);
        return false;
    }

    result = pam_acct_mgmt(pam_handle, 0); /* permitted or not? */

    result = pam_setcred(pam_handle, PAM_ESTABLISH_CRED);

    result = pam_open_session(pam_handle, 0);
    if (result != PAM_SUCCESS) {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
        error("pam_open_sesison failed.", result);
        return false;
    }
};
bool logout(void);