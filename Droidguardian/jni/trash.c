/**
 * Logs the given message to the application.
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param format message format and arguments.
 */
/*
static void LogMessage(
		JNIEnv* env,
		jobject obj,
		const char* format,
		...)
{
	// Cached log method ID
	static jmethodID methodID = NULL;

	// If method ID is not cached
	if (NULL == methodID)
	{
		// Get class from object
		jclass clazz = (*env)->GetObjectClass(env, obj);

		// Get the method ID for the given method
		methodID = (*env)->GetMethodID(env, clazz, "logMessage", "(Ljava/lang/String;)V");

		// Release the class reference
		(*env)->DeleteLocalRef(env, clazz);
	}

	// If method is found
	if (NULL != methodID)
	{
		// Format the log message
		char buffer[MAX_LOG_MESSAGE_LENGTH];

		va_list ap;
		va_start(ap, format);
		vsnprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, ap);
		va_end(ap);

		// Convert the buffer to a Java string
		jstring message = (*env)->NewStringUTF(env, buffer);

		// If string is properly constructed
		if (NULL != message)
		{
			// Log message
			(*env)->CallVoidMethod(obj, methodID, message);

			// Release the message reference
			(*env)->DeleteLocalRef(env, message);
		}
	}
}
*/

/*
 * Get user answer calling getAnswer method from Query class
 */
/*
static void getUserAnswer(
		JNIEnv* env,
		jobject obj,
		jclass clazz)
{

	// Cached log method ID
		jmethodID methodID = NULL;
		LOGD("getUserAnswer");

		// If method ID is not cached
		//if (NULL == methodID)
		//{
			// Get class from object
			//jclass clazz = (*env)->GetObjectClass(env, obj);

			// Get the method ID for the given method
			methodID = (*env)->GetMethodID(env, clazz, "getAnswer", "()V");
			if (NULL != (*env)->ExceptionOccurred(env)){
				LOGE("get answer: Get Method ID exception");
				return;
			}
			// Release the class reference
			//(*env)->DeleteLocalRef(env, clazz);
		//}

		// If method is found
		//if (NULL != methodID)
		//{

			(*env)->CallVoidMethod(env, obj, methodID);
			if (NULL != (*env)->ExceptionOccurred(env)){
				LOGE("get answer: Call Method ID error %d", errno);
				return;
			}
			LOGD("called method getAnswer");
			//(*env)->DeleteLocalRef(env, obj);
		//}
}


static void updateQueryFromKernel(
		JNIEnv* env,
		jobject obj,
		jclass clazz,
		struct dg_query *query)
{

	// Get class from object
	//jclass clazz = (*env)->GetObjectClass(env, obj);
	LOGD("updateQueryFromKernel");

	//id
	jfieldID fidId = (*env)->GetFieldID(env, clazz, "id", "I");
	if (NULL == fidId){
		LOGE("server is NULL");
		return;
	}

	jint id = (*env)->GetIntField(env, obj, fidId);
	id++;

	(*env)->SetIntField(env, obj, fidId, id);
	LOGD("SetIntField id");


	//server
	jfieldID fidServer = (*env)->GetFieldID(env, clazz, "server", "I");
	if (NULL == fidServer){
		LOGE("server is NULL");
		return;
	}

	//jint server = (*env)->GetIntField(env, obj, fidServer);
	//server = query->server;
	(*env)->SetIntField(env, obj, fidServer, query->server);
	LOGD("SetIntField server");

	//port
	jfieldID fidPort = (*env)->GetFieldID(env, clazz, "port", "I");
	if (NULL == fidPort){
		LOGE("port is NULL");
				return;
	}

	//jint port = (*env)->GetIntField(env, obj, fidPort);
	//port = query->port;
	(*env)->SetIntField(env, obj, fidPort, query->port);
	LOGD("SetIntField port");

	//pid
	jfieldID fidPid = (*env)->GetFieldID(env, clazz, "pid", "I");
	if (NULL == fidPid){
		LOGE("pid is NULL");
				return;
	}

	//jint pid = (*env)->GetIntField(env, obj, fidPid);
	//pid = query->pid;
	(*env)->SetIntField(env, obj, fidPid, query->pid);
	LOGD("SetIntField pid");

}



static void updateQueryFromUser(
		JNIEnv* env,
		jobject obj,
		jclass clazz,
		struct dg_query *query)
{
	// Get class from object
	//jclass clazz = (*env)->GetObjectClass(env, obj);
	LOGD("updateQueryFromUser");

	jfieldID fidPermission = (*env)->GetFieldID(env, clazz, "permission", "I");
	if (NULL == fidPermission){
		LOGE("permission is NULL");
		return;
	}
	jint permission = (*env)->GetIntField(env, obj, fidPermission);
	LOGD("GetIntField permission");
	query->permission = permission;

	jfieldID fidLifetime = (*env)->GetFieldID(env, clazz, "lifetime", "I");
	if (NULL == fidLifetime){
		LOGE("lifetime is NULL");
		return;
	}
	jint lifetime = (*env)->GetIntField(env, obj, fidLifetime);
	LOGD("getIntField lifetime");
	query->lifetime = lifetime;

}


static void newQuery(
		JNIEnv* env,
		struct dg_query *query,
		jobject p_context)
{
	jmethodID methodID = NULL;
	jobject obj = NULL;
	jclass clazz = NULL;

	jclass fclazz = (*env)->FindClass(env, "com/rmgoncalo/Query");
	if (fclazz == 0) {
		LOGE("Find class Query error");
		return;
	}
	else {
		LOGD("Found class Query");
	}

	clazz = (*env)->NewGlobalRef(env, fclazz);

	methodID = (*env)->GetMethodID(env, clazz, "<init>", "()V");
	if (methodID == 0){
		LOGE("Find method init error");
		return;
	}
	else {
		LOGD("Found method init");
	}

	obj = (*env)->NewObject(env, clazz, methodID);
	if (obj == NULL) {
		LOGE("obj is NULL");
		return;
	}
	else
		LOGD("obj is ok");

	//update context
	jfieldID fidContext = (*env)->GetFieldID(env, clazz, "context", "Landroid/content/Context;");
	if (NULL == fidContext){
		LOGE("context is NULL");
		return;
	} else
		LOGD("context is ok");

	//jobject ctx = (*env)->GetObjectField(env, obj, fidContext);
	(*env)->SetObjectField(env, obj, fidContext, p_context);

	updateQueryFromKernel(env, obj, clazz, query);

	getUserAnswer(env, obj, clazz);

	updateQueryFromUser(env, obj, clazz, query);

	return;
}
*/
