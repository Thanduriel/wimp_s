Meshprocessor is a command line tool that can be used to convert a variety or 3d-formats
to a reduced and simplified format that can be opened by wimp_s.
In addition it generates simple convex hulls that serve as bounding boxes for collision checks.

GENERAL USAGE

meshprocessor <filename1> <filename2>

<filename> is the full name of the 3d-model file to be processed including path and ending.
The output will be a .wim containing the mesh and a .wii file, each with the same name, as bounding mesh.
Multiple filenames can be given at once.

OPT ARGUMENTS

-nobm
No bounding mesh will be generated.

-format=<format>
Overwrites the format the mesh is saved in, default is [Flat].

-formatb=<format>
Overwrites the format the bounding mesh is saved in, default is [IndexedNormal]

-epsilon=<float>
Change the epsilon used by the convex hull generation.
This is a maximum distance a point can have from a plane to still be considered
inside of the mesh. A value close to zero generates a very detailed mesh close to the original,
a larger epsilon reduces the amount of vertices required. The default is 0.05.

FORMATS

Flat
A single array of vertices with each having a position, normal and texture coordinate.
Respectively three vertices form one triangle.
This is currently the only format that wimp_s can load as mesh.

Indexed
Currently not fully suppored.

IndexedNormal
An array of vertices with each just having a position followed by an index array,
with 3 integers forming a CCW triangle.
This is the only format wimp_s can load as bounding mesh.