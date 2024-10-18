#pragma once

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#define SERVICE_NAME "TurboVisonDM"

#define Uses_MsgBox
#include <tvision/tv.h>

static int end(int last_result);

static void init_env(struct passwd* pw);
static void set_env(char* name, char* value);

static int conv(int num_msg, const struct pam_message **msg,
                struct pam_response **resp, void *appdata_ptr);
                
static pam_handle_t *pam_handle;

bool login(const char* username, const char* password, pid_t* child_pid);
bool logout(void);

void error(char* name, int errorNo);