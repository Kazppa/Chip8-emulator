#include "chip8_emulator/os_features.h"
#include "chip8_emulator/utils.h"

#include <windows.h>
#include <shobjidl.h>

#include <iostream>

// https://docs.microsoft.com/fr-fr/windows/win32/learnwin32/example--the-open-dialog-box
std::wstring ch8::os::getFilePathDialog()
{
    std::wstring result;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!SUCCEEDED(hr)) {
        std::cerr << "Failed to open file dialog";
        return result;
    }
    // RAII cleaning
    auto initializeCleaner = utils::make_scope_callback(CoUninitialize);

    IFileOpenDialog *pFileOpen;

    // Create the FileOpenDialog object
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                          IID_IFileOpenDialog, reinterpret_cast<void **>(&pFileOpen));
    auto fileDialogCleaner = utils::make_scope_callback([pFileOpen]() { pFileOpen->Release(); });
    if (!SUCCEEDED(hr)) {
        return result;
    }

    // Show the Open dialog box
    hr = pFileOpen->Show(NULL);
    if (!SUCCEEDED(hr)) {
        return result;
    }

    // Get the file name from the dialog box
    IShellItem *pItem;
    hr = pFileOpen->GetResult(&pItem);
    if (!SUCCEEDED(hr)) {
        return result;
    }

    PWSTR pszFilePath;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    pItem->Release();

    result.assign(pszFilePath);
    return result;
}
