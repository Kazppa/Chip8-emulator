#ifndef CHIP_8_EMULATOR_OS_FEATURES_H
#define CHIP_8_EMULATOR_OS_FEATURES_H

#include <string>

namespace ch8::os
{
    // Open file dialog to request a file path
    std::wstring getFilePathDialog();
}

#endif //CHIP_8_EMULATOR_OS_FEATURES_H
