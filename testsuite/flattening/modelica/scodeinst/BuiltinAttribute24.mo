// name: BuiltinAttribute24
// keywords:
// status: incorrect
//

model BuiltinAttribute24
  Real x[2,2](start = {3,1});
end BuiltinAttribute24;

// Result:
// Error processing file: BuiltinAttribute24.mo
// [flattening/modelica/scodeinst/BuiltinAttribute24.mo:7:15-7:28:writable] Notification: From here:
// [flattening/modelica/scodeinst/BuiltinAttribute24.mo:7:3-7:29:writable] Error: Type mismatch in binding 'start = {3.0, 1.0}', expected array dimensions [2, 2], got [2].
//
// # Error encountered! Exiting...
// # Please check the error message and the flags.
//
// Execution failed!
// endResult
