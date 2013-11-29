#include "com_rmgoncalo_Query.h"

#include <jni.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stddef.h>

// Max log message length
#define MAX_LOG_MESSAGE_LENGTH 256

#define SOCK_PATH "/data/data/com.rmgoncalo/dgsocket"

struct dg_query{
	int32_t server;
	int32_t port;
	int32_t pid;
};

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
		methodID = (*env)->GetMethodID(env, clazz, "logMessage",
				"(Ljava/lang/String;)V");

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

/**
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

	int localSocket = socket(PF_LOCAL, SOCK_STREAM, 0);

	// Check if socket is properly constructed
	if (-1 == localSocket)
	{
		// Throw an exception with error number
		ThrowErrnoException(env, "java/io/IOException", errno);
	}

	return localSocket;
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
		socklen_t addressLength =
				sizeof(address);

		// Unlink if the socket name is already binded
		unlink(address.sun_path);

		// Bind socket
		//LogMessage(env, obj, "Binding to local name %s.",
		//		SOCK_PATH);

		if (-1 == bind(sd, (struct sockaddr*) &address, addressLength))
		{
			// Throw an exception with error number
			ThrowErrnoException(env, "java/io/IOException", errno);
		}
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
	}

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

	ssize_t recvSize = recv(sd, query, query_size, 0);

	// If receive is failed
	if (-1 == recvSize)
	{
		// Throw an exception with error number
		ThrowErrnoException(env, "java/io/IOException", errno);
	}
	else
	{
		// If data is received
		if (recvSize > 0)
		{
			//LogMessage(env, obj, "Received %d bytes.", recvSize);
		}
		else
		{
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
	ssize_t sentSize = send(sd, query, dg_query, 0);

	// If send is failed
	if (-1 == sentSize)
	{
		// Throw an exception with error number
		ThrowErrnoException(env, "java/io/IOException", errno);
	}
	else
	{
		if (sentSize > 0)
		{
			//LogMessage(env, obj, "Sent %d bytes.", sentSize);
		}
		else
		{
			//LogMessage(env, obj, "Client disconnected.");
		}
	}

	return sentSize;
}

void Java_com_rmgoncalo_Query_updateVars(
		JNIEnv* env,
		jobject obj)
{
	// Construct a new local UNIX socket.

	int serverSocket = NewLocalSocket(env, obj);
	if (NULL == (*env)->ExceptionOccurred(env))
	{

		// Bind socket to a port number
		BindLocalSocket(env, obj, serverSocket);

		// If bind is failed
		if (NULL != (*env)->ExceptionOccurred(env))
			goto exit;

		// Listen on socket with a backlog of 4 pending connections
		ListenOnSocket(env, obj, serverSocket, 4);
		if (NULL != (*env)->ExceptionOccurred(env))
			goto exit;

		// Accept a client connection on socket
		int clientSocket = AcceptOnLocalSocket(env, obj, serverSocket);
		if (NULL != (*env)->ExceptionOccurred(env))
			goto exit;

		struct dg_query* query;
		query = (struct dg_query *) malloc(sizeof(struct dg_query));
		size_t query_size = sizeof(struct dg_query);
		ssize_t recvSize;
		//ssize_t sentSize;

		// Receive and send back the data
		while (1)
		{
			// Receive from the socket
			recvSize = ReceiveFromSocket(env, obj, clientSocket,
					query, query_size);

			if ((0 == recvSize) || (NULL != (*env)->ExceptionOccurred(env)))
				break;

			jclass thisClass = (*env)->GetObjectClass(env, obj);

				//server
				jfieldID fidServer = (*env)->GetFieldID(env, thisClass, "server", "I");
				if (NULL == fidServer) return;
				jint server = (*env)->GetIntField(env, obj, fidServer);
				server = query->server;
				(*env)->SetIntField(env, obj, fidServer, server);

				//port
				jfieldID fidPort = (*env)->GetFieldID(env, thisClass, "port", "I");
				if (NULL == fidPort) return;
				jint port = (*env)->GetIntField(env, obj, fidPort);
				port = query->port;
				(*env)->SetIntField(env, obj, fidPort, port);

				//pid
				jfieldID fidPid = (*env)->GetFieldID(env, thisClass, "pid", "I");
				if (NULL == fidPid) return;
				jint pid = (*env)->GetIntField(env, obj, fidPid);
				pid = query->pid;
				(*env)->SetIntField(env, obj, fidPid, pid);


			// Send to the socket
			//sentSize = SendToSocket(env, obj, clientSocket,
			//		query, (size_t) recvSize);

			//if ((0 == sentSize) || (NULL != (*env)->ExceptionOccurred(env)))
			//	break;
		}

		// Close the client socket
		close(clientSocket);
	}

exit:
	if (serverSocket > 0)
	{
		close(serverSocket);
	}
}
