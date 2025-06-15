/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   Ball_tab_0_svg;
    const int            Ball_tab_0_svgSize = 13472;

    extern const char*   Ball_tab_1_svg;
    const int            Ball_tab_1_svgSize = 13470;

    extern const char*   Ball_tab_2_svg;
    const int            Ball_tab_2_svgSize = 13478;

    extern const char*   ByKnobs_svg;
    const int            ByKnobs_svgSize = 317733;

    extern const char*   ByTempo_svg;
    const int            ByTempo_svgSize = 318123;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 5;

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
