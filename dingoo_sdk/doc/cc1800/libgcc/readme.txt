libgcc.a is from the mp4sdk project.
However, it has been repackaged, and some functions replaced, to allow 64bit divisions.

The following functions:

__divdi3
__udivdi3
__umoddi3

has been replaced by _div64 which contains these.



After extracting the object files from the old libgcc and removed libgcc.a, 
package_libgcc.bat can be used.