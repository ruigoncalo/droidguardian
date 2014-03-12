#include "com_rmgoncalo_droidg_Daemon.h"
#include "sockets.h"

#include <android/log.h>
#include <jni.h>
#include <time.h>

// Max log message length
#define MAX_LOG_MESSAGE_LENGTH 256

#define  LOG_TAG    "DG-Native"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define BILLION 1E9
struct timespec requestStart, requestEnd;
struct timespec inner_rStart, inner_rEnd;

/*
 * Throws a new exception using the given exception class
 * and exception message.
 *
 * @param env JNIEnv interface.
 * @param className class name.
 * @param message exception message.
 */

static void ThrowException(
		JNIEnv* env,
		const char* className,
		const char* message)
{
	// Get the exception class
	jclass clazz = (*env)->FindClass(env, className);

	// If exception class is found
	if (NULL != clazz)
	{
		// Throw exception
		(*env)->ThrowNew(env, clazz, message);
		LOGD("Exception thrown: %s",message);

		// Release local class reference
		(*env)->DeleteLocalRef(env, clazz);
	}
}

/**
 * Throws a new exception using the given exception class
 * and error message based on the error number.
 *
 * @param env JNIEnv interface.
 * @param className class name.
 * @param errnum error number.
 */
static void ThrowErrnoException(
		JNIEnv* env,
		const char* className,
		int errnum)
{
	char buffer[MAX_LOG_MESSAGE_LENGTH];

	// Get message for the error number
	if (-1 == strerror_r(errnum, buffer, MAX_LOG_MESSAGE_LENGTH))
	{
		strerror_r(errno, buffer, MAX_LOG_MESSAGE_LENGTH);
	}

	// Throw exception
	ThrowException(env, className, buffer);
}


/**
 * Constructs a new Local UNIX socket.
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @return socket descriptor.
 * @throws IOException
 */
static int NewLocalSocket(JNIEnv* env, jobject obj)
{
	// Construct socket
	//LogMessage(env, obj, "Constructing a new Local UNIX socket...");

	int localSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	LOGD("New local socket");

	// Check if socket is properly constructed
	if (-1 == localSocket)
	{
		// Throw an exception with error number
		ThrowErrnoException(env, "java/io/IOException", errno);
		LOGE("New local socket error");
	}

	return localSocket;
}

/**
 * Connects a new Local UNIX socket (abstract namespace).
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param socket descriptor.
 * @throws IOException
 */
static void ConnectToSocket(JNIEnv* env, jobject obj, int sd)
{
	struct sockaddr_un address;
	socklen_t len;

	memset(&address, 0, sizeof(address));
	address.sun_family = AF_UNIX;

	// first char must be have 0 bytes - abstract namespace
	address.sun_path[0] = '\0';
	strcpy(&address.sun_path[1], SOCKJAVA_PATH );
	len = offsetof(struct sockaddr_un, sun_path) + 1 + strlen(&address.sun_path[1]);

	// Unlink if the socket name is already binded
	unlink(address.sun_path);

	// Connect to address
	if (-1 == connect(sd, (struct sockaddr*) &address, len))
	{
		// Throw an exception with error number
		ThrowErrnoException(env, "java/io/IOException", errno);
	}
}

/**
 * Binds a local UNIX socket to a name.
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param sd socket descriptor.
 * @param name socket name.
 * @throws IOException
 */
static void BindLocalSocket(
		JNIEnv* env,
		jobject obj,
		int sd)
{
	struct sockaddr_un address;

		// Clear the address bytes
		memset(&address, 0, sizeof(address));
		address.sun_family = AF_UNIX;

		// Socket path
		char* sunPath = address.sun_path;

		// Append the local name
		strcpy(sunPath, SOCK_PATH);

		// Address length
		socklen_t addressLength = sizeof(address);

		// Unlink if the socket name is already binded
		unlink(address.sun_path);

		// Bind socket
		//LogMessage(env, obj, "Binding to local name %s.",
		//		SOCK_PATH);

		if (-1 == bind(sd, (struct sockaddr*) &address, addressLength))
		{
			// Throw an exception with error number
			ThrowErrnoException(env, "java/io/IOException", errno);
			LOGE("Bind local socket error");
		}

		LOGD("Socket bind");
}


/**
 * Listens on given socket with the given backlog for
 * pending connections. When the backlog is full, the
 * new connections will be rejected.
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param sd socket descriptor.
 * @param backlog backlog size.
 * @throws IOException
 */
static void ListenOnSocket(
		JNIEnv* env,
		jobject obj,
		int sd,
		int backlog)
{
	// Listen on socket with the given backlog
	//LogMessage(env, obj,
	//		"Listening on socket with a backlog of %d pending connections.",
	//		backlog);

	if (-1 == listen(sd, backlog))
	{
		// Throw an exception with error number
		ThrowErrnoException(env, "java/io/IOException", errno);
		LOGE("Socket listen error");
	}

	LOGD("Socket listen");
}

/**
 * Blocks and waits for incoming client connections on the
 * given socket.
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param sd socket descriptor.
 * @return client socket.
 * @throws IOException
 */
static int AcceptOnLocalSocket(
		JNIEnv* env,
		jobject obj,
		int sd)
{
	// Blocks and waits for an incoming client connection
	// and accepts it
	//LogMessage(env, obj, "Waiting for a client connection...");

	int clientSocket = accept(sd, NULL, NULL);

	// If client socket is not valid
	if (-1 == clientSocket)
	{
		// Throw an exception with error number
		ThrowErrnoException(env, "java/io/IOException", errno);
		LOGE("Accept socket error");
	}

	LOGD("Accept socket");
	return clientSocket;
}

/**
 * Block and receive data from the socket into the buffer.
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param sd socket descriptor.
 * @param buffer data buffer.
 * @param bufferSize buffer size.
 * @return receive size.
 * @throws IOException
 */
static ssize_t ReceiveFromSocket(
		JNIEnv* env,
		jobject obj,
		int sd,
		struct dg_query* query,
		size_t query_size)
{
	// Block and receive data from the socket into the buffer
	//LogMessage(env, obj, "Receiving from the socket...");

	ssize_t recvSize = read(sd, query, query_size);

	// If receive is failed
	if (-1 == recvSize)
	{
		// Throw an exception with error number
		ThrowErrnoException(env, "java/io/IOException", errno);
		LOGE("Receive size error");
	}
	else
	{
		// If data is received
		if (recvSize > 0)
		{
			LOGD("Received %d bytes", (int) recvSize);
			//LogMessage(env, obj, "Received %d bytes.", recvSize);
		}
		else
		{
			LOGD("Receive: client disconnected");
			//LogMessage(env, obj, "Client disconnected.");
		}
	}

	return recvSize;
}

/**
 * Send data buffer to the socket.
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param sd socket descriptor.
 * @param buffer data buffer.
 * @param bufferSize buffer size.
 * @return sent size.
 * @throws IOException
 */
static ssize_t SendToSocket(
		JNIEnv* env,
		jobject obj,
		int sd,
		struct dg_query* query,
		size_t dg_query)
{

	// Send data buffer to the socket
	//LogMessage(env, obj, "Sending to the socket...");
	ssize_t sentSize = write(sd, query, dg_query);

	// If send is failed
	if (-1 == sentSize)
	{
		// Throw an exception with error number
		ThrowErrnoException(env, "java/io/IOException", errno);
		LOGE("Send size error");
	}
	else
	{
		if (sentSize > 0)
		{
			LOGD("Sent %d bytes.", (int) sentSize);
			//LogMessage(env, obj, "Sent %d bytes.", sentSize);
		}
		else
		{
			LOGD("Send: client disconnected");
			//LogMessage(env, obj, "Client disconnected.");
		}
	}

	return sentSize;
}

int get_proc_name_from_pid(int pid, char proc[])
{
	FILE *file;
	int err, i;

	char tmp[10];
	char path[PROCESS_SIZE]="/proc/";

	sprintf(tmp, "%d", pid);
	strcat(path, tmp);
	strcat(path, "/cmdline");

	if ((file = fopen(path, "r")) == NULL)
	{
		LOGE("Error opening proc/pid/cmdline.");
		return -1;
	}

	if ( fgets(proc ,PROCESS_SIZE ,file) == NULL )
	{
		LOGE("Error getting cmdline");
		return -1;
	}

	fclose(file);

	return 0;
}

/**
 * Send query to java and gets answer
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param struct sockaddr_in addrin.
 * @param int process id.
 * @param char[] process name.
 * @return permission value.
 */
static int query4_to_java(
		JNIEnv* env,
		jobject obj,
		struct sockaddr_in addrin,
		int pid)
{
	int sendm, recvm, result = -1;
	char *recvs;
	char msg[INET_ADDRSTRLEN + PORT_SIZE + INT_SIZE + PROCESS_SIZE];
	char sport[PORT_SIZE];
	char spid[INT_SIZE];
	char process[PROCESS_SIZE];
	char ip[INET_ADDRSTRLEN];
	unsigned short port;

	recvs = (char *) malloc ( sizeof ((*recvs) * INT_SIZE));

	inet_ntop(AF_INET, &(addrin.sin_addr), ip, INET_ADDRSTRLEN);

	port = ntohs(addrin.sin_port);
	sprintf(sport, "%hu", port);

	sprintf(spid, "%d", pid);

	if(get_proc_name_from_pid(pid, process) == -1)
	{
		// In case /proc/pid/cmdline fails, process name is "Unknown"
		strcpy(process, "Unknown");
	}

	LOGD("ip: %s, port: %d, pid: %d\n", ip, port, pid);


	int clientSocket = NewLocalSocket(env, obj);
	if (NULL == (*env)->ExceptionOccurred(env))
	{
		ConnectToSocket(env, obj, clientSocket);

		// string will be parsed in java
		// ATENTION: ';' is used to split the string
		snprintf(msg, sizeof(msg), "%s;%s;%s;%s", ip, sport, spid, process);

		// talk to java using write and read
		sendm = write(clientSocket, msg, strlen(msg));
		LOGD("Total write = %d", sendm);

		// receives 1 byte from the java
		recvm = read(clientSocket, recvs, 1);
		LOGD("Total read = %d", recvm);

		recvs[recvm] = '\0';
		result = atoi(recvs);
	}

	if(clientSocket > 0)
		close(clientSocket);

	return result;
}

/**
 * Send query to java and gets answer
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param struct sockaddr_in6 addrin6.
 * @param int process id.
 * @param char[] process name.
 * @return permission value.
 */
static int query6_to_java(
		JNIEnv* env,
		jobject obj,
		struct sockaddr_in6 addrin6,
		int pid)
{
	int sendm, recvm, result = -1;
	char *recvs;
	char msg[INET6_ADDRSTRLEN + PORT_SIZE + INT_SIZE + PROCESS_SIZE];
	char sport[PORT_SIZE];
	char spid[INT_SIZE];
	char process[PROCESS_SIZE];
	char ip6[INET6_ADDRSTRLEN];
	unsigned short port;

	recvs = (char *) malloc ( sizeof ((*recvs) * INT_SIZE));

	inet_ntop(AF_INET6, &(addrin6.sin6_addr), ip6, INET6_ADDRSTRLEN);

	port = ntohs(addrin6.sin6_port);
	sprintf(sport, "%hu", port);

	sprintf(spid, "%d", pid);

	if(get_proc_name_from_pid(pid, process) == -1)
	{
		// In case /proc/pid/cmdline fails, process name is "Unknown"
		strcpy(process, "Unknown");
	}

	LOGD("ip: %s, port: %d, pid: %d\n", ip6, port, pid);

	int clientSocket = NewLocalSocket(env, obj);
	if (NULL == (*env)->ExceptionOccurred(env))
	{
		ConnectToSocket(env, obj, clientSocket);

		// string will be parsed in java
		// ATENTION: ';' is used to split the string
		snprintf(msg, sizeof(msg), "%s;%s;%s;%s", ip6, sport, spid, process);

		// talk to java using write and read
		sendm = write(clientSocket, msg, strlen(msg));
		//LOGD("Total write = %d", sendm);

		// receives 1 byte from the java
		recvm = read(clientSocket, recvs, 1);
		//LOGD("Total read = %d", recvm);

		recvs[recvm] = '\0';
		result = atoi(recvs);
	}

	if(clientSocket > 0)
		close(clientSocket);

	return result;
}

/*
 * Handle answer from app
 */
int getResult(int result)
{
	switch(result)
	{
		case ALLOW_FOREVER:
			return ALLOW;
			break;

		case DENY_FOREVER:
			return DENY;
			break;

		case ALLOW_ONCE:
			return ALLOW;
			break;

		case DENY_ONCE:
			return DENY;
			break;

		// in case of error the access to the Internet is allowed
		default:
			return ALLOW;
	}
}

/*
 * run daemon
 */

void Java_com_rmgoncalo_droidg_Daemon_startDaemon(
		JNIEnv* env,
		jobject obj)
{
	clock_gettime(CLOCK_REALTIME, &requestStart);

	struct dg_query* query;
	int result = 1;
	int mode, i;

	query = (struct dg_query *) malloc(sizeof(struct dg_query));
	size_t query_size = sizeof(struct dg_query);
	ssize_t recvSize, sentSize;

	// Construct a new local UNIX socket.

	int serverSocket = NewLocalSocket(env, obj);
	if (NULL == (*env)->ExceptionOccurred(env))
	{

		// Bind socket to a port number
		BindLocalSocket(env, obj, serverSocket);

		// If bind is failed
		if (NULL != (*env)->ExceptionOccurred(env))
			goto exit;

		//Change socket permissions
		mode = chmod(SOCK_PATH, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if(mode == -1)
		{
			LOGE("Error chmod");
			goto exit;
		}

		// Listen on socket with a backlog of 1024 pending connections
		ListenOnSocket(env, obj, serverSocket, 1024);
		if (NULL != (*env)->ExceptionOccurred(env))
			goto exit;

		for(i = 0; i < 25; i++)
		{
			clock_gettime(CLOCK_REALTIME, &inner_rStart);

			// Accept a client connection on socket
			int clientSocket = AcceptOnLocalSocket(env, obj, serverSocket);
			if (NULL != (*env)->ExceptionOccurred(env))
				break;

			// Receive query from kernel
			recvSize = ReceiveFromSocket(env, obj, clientSocket, query, query_size);
			if ((0 == recvSize) || (NULL != (*env)->ExceptionOccurred(env)))
				break;

			//query to java
			if(recvSize > 0)
			{
				switch(query->family)
				{
					case DG_INET:
						result = query4_to_java(env, obj, query->addrin, query->pid);
						break;
					case DG_INET6:
						result = query6_to_java(env, obj, query->addrin6, query->pid);
						break;
					default:
						LOGE("Query family error!");
				}
			}

			query->permission = getResult(result);

			// Send to the socket
			sentSize = SendToSocket(env, obj, clientSocket, query,  (size_t) recvSize);
			if ((0 == sentSize) || (NULL != (*env)->ExceptionOccurred(env)))
				break;

			// Close the client socket
			close(clientSocket);

			clock_gettime(CLOCK_REALTIME, &inner_rEnd);

			double iaccum = ( inner_rEnd.tv_sec - inner_rStart.tv_sec )
				  + ( inner_rEnd.tv_nsec - inner_rStart.tv_nsec )
				  / BILLION;
			LOGD( "inner time %d: %lf\n", i, iaccum );
		}
	}

	clock_gettime(CLOCK_REALTIME, &requestEnd);

	double accum = ( requestEnd.tv_sec - requestStart.tv_sec )
	  + ( requestEnd.tv_nsec - requestStart.tv_nsec )
	  / BILLION;
	LOGD( "total time: %lf\n", accum );

exit:
	if (serverSocket > 0)
	{
		close(serverSocket);
	}
}

