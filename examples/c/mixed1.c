/*
*    ------ BEGIN LICENSE ATTRIBUTION ------
*    
*    Portions of this file have been appropriated or derived from the following project(s) and therefore require attribution to the original licenses and authors.
*    
*    Repository: https://github.com/eclipse/mosquitto
*    Source File: src/control.c
*    Licenses:
*      Eclipse Public License 2.0
*      SPDXId: EPL-2.0
*    
*    Auto-attribution by Threatrix, Inc.
*    
*    ------ END LICENSE ATTRIBUTION ------
*/
// https://github.com/eclipse/mosquitto/blob/v2.0.1/src/control.c
#ifdef WITH_CONTROL
/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
int control__process(struct mosquitto *context, struct mosquitto_msg_store *stored)
{
	struct mosquitto__callback *cb_found;
	struct mosquitto_evt_control event_data;
	struct mosquitto__security_options *opts;
	mosquitto_property *properties = NULL;
	int rc = MOSQ_ERR_SUCCESS;

	if(db.config->per_listener_settings){
		opts = &context->listener->security_options;
	}else{
		opts = &db.config->security_options;
	}
	HASH_FIND(hh, opts->plugin_callbacks.control, stored->topic, strlen(stored->topic), cb_found);
	if(cb_found){
		memset(&event_data, 0, sizeof(event_data));
		event_data.client = context;
		event_data.topic = stored->topic;
		event_data.payload = stored->payload;
		event_data.payloadlen = stored->payloadlen;
		event_data.qos = stored->qos;
		event_data.retain = stored->retain;
		event_data.properties = stored->properties;
		event_data.reason_code = MQTT_RC_SUCCESS;
		event_data.reason_string = NULL;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_data, cb_found->userdata);
		if(rc){
			if(context->protocol == mosq_p_mqtt5 && event_data.reason_string){
				mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_data.reason_string);
			}
		}
		free(event_data.reason_string);
		event_data.reason_string = NULL;
	}

	if(stored->qos == 1){
		if(send__puback(context, stored->source_mid, MQTT_RC_SUCCESS, properties)) rc = 1;
	}else if(stored->qos == 2){
		if(send__pubrec(context, stored->source_mid, MQTT_RC_SUCCESS, properties)) rc = 1;
	}
	mosquitto_property_free_all(&properties);

	return rc;
}
#endif

// https://github.com/eclipse/mosquitto/blob/v1.5.3/src/mosquitto.c
int drop_privileges(struct mosquitto__config *config, bool temporary)
{
#if !defined(__CYGWIN__) && !defined(WIN32)
	struct passwd *pwd;
	char err[256];
	int rc;

	const char *snap = getenv("SNAP_NAME");
	if(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		return MOSQ_ERR_SUCCESS;
	}

	if(geteuid() == 0){
		if(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			if(!pwd){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid user '%s'.", config->user);
				return 1;
			}
			if(initgroups(config->user, pwd->pw_gid) == -1){
				strerror_r(errno, err, 256);
				log__printf(NULL, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				return 1;
			}
			if(temporary){
				rc = setegid(pwd->pw_gid);
			}else{
				rc = setgid(pwd->pw_gid);
			}
			if(rc == -1){
				strerror_r(errno, err, 256);
				log__printf(NULL, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				return 1;
			}
			if(temporary){
				rc = seteuid(pwd->pw_uid);
			}else{
				rc = setuid(pwd->pw_uid);
			}
			if(rc == -1){
				strerror_r(errno, err, 256);
				log__printf(NULL, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				return 1;
			}
		}
		if(geteuid() == 0 || getegid() == 0){
			log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}
#endif
	return MOSQ_ERR_SUCCESS;
}
// https://github.com/zephyrproject-rtos/zephyr/blob/zephyr-v1.3.0/arch/arm/core/sys_fatal_error_handler.c
void _SysFatalErrorHandler(unsigned int reason, const NANO_ESF * pEsf)
{
	nano_context_type_t curCtx = sys_execution_context_type_get();

	ARG_UNUSED(reason);
	ARG_UNUSED(pEsf);

	if ((curCtx == NANO_CTX_ISR) || _is_thread_essential(NULL)) {
		PRINTK("Fatal fault in %s ! Spinning...\n",
		       NANO_CTX_ISR == curCtx
			       ? "ISR"
			       : NANO_CTX_FIBER == curCtx ? "essential fiber"
							  : "essential task");
		for (;;)
			; /* spin forever */
	}

	if (NANO_CTX_FIBER == curCtx) {
		PRINTK("Fatal fault in fiber ! Aborting fiber.\n");
		fiber_abort();
		return;
	}

	NON_ESSENTIAL_TASK_ABORT();
}




