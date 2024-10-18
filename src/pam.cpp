#include "pam.h"
#include <pwd.h>
#include <security/pam_appl.h>

static void init_env(struct passwd* pw)
{
    set_env("HOME", pw->pw_dir);
    set_env("PWD", pw->pw_dir);
    set_env("SHELL", pw->pw_shell);
    set_env("USER", pw->pw_name);
    set_env("LOGNAME", pw->pw_name);
    set_env("PATH", "/usr/local/sbin:/usr/local/bin:/usr/bin:/bin");
    set_env("DISPLAY", ":0");
};

static void set_env(char* name, char* value)
{
    // <name>=<value>
    char* what_to_set;
    sprintf(what_to_set, "");
    pam_putenv(pam_handle, what_to_set);
};

static int end(int last_result)
{
    int result = pam_end(pam_handle, last_result);
    pam_handle = 0;
    return result;
};

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
    if (result != PAM_SUCCESS)
    {
        error("pan_acct_mgmt failed.", result);
        return false;
    }

    result = pam_setcred(pam_handle, PAM_ESTABLISH_CRED);
    if (result != PAM_SUCCESS)
    {
        error("pam_setcred failed.", result);
        return false;
    }

    result = pam_open_session(pam_handle, 0);
    if (result != PAM_SUCCESS) {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
        error("pam_open_sesison failed.", result);
        return false;
    }
};

bool logout(void)
{
    int result = pam_close_session(pam_handle, 0);
    
    if (result != PAM_SUCCESS)
    {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
        error("pam_close_session", result);
    }

    result = pam_setcred(pam_handle, PAM_DELETE_CRED);
    if (result != PAM_SUCCESS)
    {
        error("pam_setcred - PAM_DELETE_CRED", result);
    }

    end(result);
    return true;
};

static int conv(int num_msg, const struct pam_message **msg,
                 struct pam_response **resp, void *appdata_ptr) {
    int i;

    *resp = static_cast<pam_response*>(calloc(num_msg, sizeof(struct pam_response)));
    if (*resp == NULL) {
        return PAM_BUF_ERR;
    }

    int result = PAM_SUCCESS;
    char *username, *password;

    for (i = 0; i < num_msg; i++) {
        switch (msg[i]->msg_style) {
            case PAM_PROMPT_ECHO_ON:
                username = ((char **) appdata_ptr)[0];
                (*resp)[i].resp = strdup(username);
                break;
            case PAM_PROMPT_ECHO_OFF:
                password = ((char **) appdata_ptr)[1];
                (*resp)[i].resp = strdup(password);
                break;
            
            case PAM_ERROR_MSG:
                fprintf(stderr, "%s\n", msg[i]->msg);
                result = PAM_CONV_ERR;
                break;

            case PAM_TEXT_INFO:
                printf("%s\n", msg[i]->msg);
                break;
        }
        if (result != PAM_SUCCESS) {
            break;
        }
    }

    if (result != PAM_SUCCESS) {
        free(*resp);
        *resp = 0;
    }

    return result;
}

void error(char* name, int errorNo)
{
    char* buffer;
    sprintf(buffer, "%s: %s", name, pam_strerror(pam_handle, errorNo));
    end(errorNo);
    messageBox(buffer, mfError | mfOKButton);
}