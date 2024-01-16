////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_LSPERRORCODES_H
#define __APP_LSPERRORCODES_H


////////////////////////////////////////////////////////////////////////////////////////////////////
// Error codes as defined by the Language Server Protocol
////////////////////////////////////////////////////////////////////////////////////////////////////
class LSPErrorCodes {

public:
	/**
	 * Error codes defined by JSON-RPC
	 */
	static int ParseError;
	static int InvalidRequest;
	static int MethodNotFound;
	static int InvalidParams;
	static int InternalError;

	/**
	 * Error code indicating that a server received a notification or
	 * request before the server has received the `initialize` request.
	 */
	static int ServerNotInitialized;

	/**
	 * Error code for unknown errors
	 */
	static int UnknownErrorCode;

	/**
	 * A request failed but it was syntactically correct, e.g the
	 * method name was known and the parameters were valid. The error
	 * message should contain human readable information about why
	 * the request failed.
	 *
	 * @since 3.17.0
	 */
	static int RequestFailed;

	/**
	 * The server cancelled the request. This error code should
	 * only be used for requests that explicitly support being
	 * server cancellable.
	 *
	 * @since 3.17.0
	 */
	static int ServerCancelled;

	/**
	 * The server detected that the content of a document got
	 * modified outside normal conditions. A server should
	 * NOT send this error code if it detects a content change
	 * in it unprocessed messages. The result even computed
	 * on an older state might still be useful for the client.
	 *
	 * If a client decides that a result is not of any use anymore
	 * the client should cancel the request.
	 */
	static int ContentModified;

	/**
	 * The client has canceled a request and a server as detected
	 * the cancel.
	 */
	static int RequestCancelled;

private:
	LSPErrorCodes() = default;
};

#endif

