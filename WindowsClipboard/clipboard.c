#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <ctype.h>

// Definicja callbacka
typedef void (*Callback)(const char*);

// Zmienna globalna do przechowywania ostatnio skopiowanego tekstu
char lastCopiedText[256] = "";

// Funkcja do odwracania tekstu
void reverseText(const char* text) {
    int len = strlen(text);
    char reversed[256];
    for (int i = 0; i < len / 2; i++) {
        char tmp = text[i];
        reversed[i] = text[len - i - 1];
        reversed[len - i - 1] = tmp;
    }
    reversed[len] = '\0';
}

int checkTextFormat(const char *text) {
    int textLen = 32;
    int spacesIdx[6] = {2, 7, 12, 17, 22, 27};
    int digitsIdx[26] = {0, 1, 3, 4, 5, 6, 8, 9, 10, 11, 13, 14, 15, 16, 18, 19, 20, 21, 23, 24, 25, 26, 28, 29, 30, 31};

    int len = strlen(text);
    if (len < textLen) {
        return 0;
    }

    for (int i = 0; i < 26; i++) {
        if (!isdigit(text[digitsIdx[i]])) {
            return 0;
        }
    }

    for (int i = 0; i < 6; i++) {
        if ((text[spacesIdx[i]] != ' ')) {
            return 0;
        }
    }

    return 1;
}

// Funkcja do odczytywania tekstu ze schowka i wywołania callbacka
void readClipboard(Callback callback) {
    HWND hwnd = NULL; // Uchwyt do okna, można ustawić na NULL
    HANDLE hGlMem = NULL; // Uchwyt do bloku pamięci globalnej
    LPSTR lpGlMem = NULL; // Wskaźnik do bloku pamięci globalnej
    char text[256]; // Bufor na tekst z schowka

    if (IsClipboardFormatAvailable(CF_TEXT)) { // Sprawdzamy, czy schowek zawiera tekst w formacie CF_TEXT
        OpenClipboard(hwnd); // Otwieramy schowek
        HANDLE hCbMem = GetClipboardData(CF_TEXT); // Pobieramy uchwyt do bloku pamięci globalnej z tekstem ze schowka
        LPSTR lpCbMem = (LPSTR)GlobalLock(hCbMem); // Blokujemy dostęp do bloku pamięci globalnej

        // Odczytujemy tekst ze schowka
        strncpy(text, lpCbMem, 255);
        text[255] = '\0';

        GlobalUnlock(hCbMem); // Odblokowujemy dostęp do bloku pamięci globalnej
        CloseClipboard(); // Zamykamy schowek

        // Sprawdzamy, czy odczytany tekst jest inny od ostatnio skopiowanego tekstu
        if (checkTextFormat(text)) {            // Wywołujemy callback z odczytanym tekstem
            callback(text);

            // Modyfikujemy tekst (odwracamy go)
            int len = strlen(text);
            for (int i = 0; i < len / 2; i++) {
                char tmp = text[i];
                text[i] = text[len - i - 1];
                text[len - i - 1] = tmp;
            }

            // Przygotowujemy nowy blok pamięci globalnej z zmodyfikowanym tekstem
            hGlMem = GlobalAlloc(GHND, len + 1);
            lpGlMem = (LPSTR)GlobalLock(hGlMem);
            strncpy(lpGlMem, text, len + 1);
            GlobalUnlock(hGlMem);

            OpenClipboard(hwnd); // Otwieramy schowek
            EmptyClipboard();
            SetClipboardData(CF_TEXT, hGlMem); // Ustawiamy nowy blok pamięci globalnej jako zawartość schowka
            CloseClipboard(); // Zamykamy schowek

            strncpy(lastCopiedText, text, 255); // Aktualizujemy ostatnio skopiowany tekst
        }
    }
}

// Funkcja callback do przekazania do funkcji readClipboard
void clipboardCallback(const char* text) {
    reverseText(text); // Wywołujemy funkcję odwracająca tekst
}

int main() {

    while (1) {
        readClipboard(clipboardCallback); // Odczytujemy tekst ze schowka i wywołujemy callback
        Sleep(1000); // Czekamy 1 sekundę
    }

    return 0;
}
