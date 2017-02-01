@namespace ubiq

@brief Mixed utilities that can come in handy in any context.

The _ubiq_ namespace contains several completely seperate tools:

 - The Failable class end its descendants, usable for ensuring the presence of error checking code, in test builds.
 - The Trace class, which implements a compile time parametrizable filter and router for debugging info.
 - The Compiler.h header that is used for various compiler hacks.

DebugConfig.h
-------------

The application is required to provde a haader file called DebugConfig.h in the include path when compiling a 
compilation unit that uses any of the Trace or Failable utilities.
Although the presence of this file is required it can be left completely empty, in that case the tools resort to 
the default configuration, which is not to generate any code.
In this file the user can configure several build time resolved properties of these tools.
The settings are provided through macros from the TraceCommon.h and FailableCommon.h header that can be used in this configuration file.

The failable utility has a single configuration option: the code to be run when an unchecked error is detected.

@code
UNCHECKED_ERROR_REPORT() {
	// printf, abort or something();
}
@endcode

The trace utility has three options options:
 - Output writer: the _TRACE_WRITER_ macro, requires a single paramater that is the nam of the class that provides the output writer functionality.
 - Default log level: _GLOBAL_TRACE_POLICY_, that receives a single parameter that is the default level specified by a _Level_ value.
 - Per-client log level has two forms with two and three parameters:
   - CLIENT_TRACE_POLICY: first parameter is the class that identifies the trace channel, and then the log level from the _Level_ enumeration.
   - CLIENT_TRACE_POLICY_NS: first a namespace, then the tag class and finally the level.

@code
TRACE_WRITER(MockWriter);
GLOBAL_TRACE_POLICY(Failure);
CLIENT_TRACE_POLICY(B, Information);
CLIENT_TRACE_POLICY_NS(ns, C, Warning);
@endcode
