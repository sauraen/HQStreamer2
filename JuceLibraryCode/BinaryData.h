/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   hqs2_icon_16_png;
    const int            hqs2_icon_16_pngSize = 569;

    extern const char*   hqs2_icon_32_png;
    const int            hqs2_icon_32_pngSize = 867;

    extern const char*   hqs2_icon_64_png;
    const int            hqs2_icon_64_pngSize = 1441;

    extern const char*   hqs2_icon_256_png;
    const int            hqs2_icon_256_pngSize = 5530;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 4;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
