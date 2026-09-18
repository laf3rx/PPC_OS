#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Keypad.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <LittleFS.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <esp_system.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <mbedtls/aes.h>







#define TFT_CS    22
#define TFT_DC    16
#define TFT_RST   17
#define TFT_SCK   18
#define TFT_MOSI  5

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);
U8G2_FOR_ADAFRUIT_GFX u8g2;


const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 14, 25, 26};
byte colPins[COLS] = {27, 32, 33, 23};
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);




enum ThemeId {
  THEME_CLASSIC = 0,
  THEME_AMBER,
  THEME_COUNT
};

uint8_t currentTheme = THEME_CLASSIC;



uint16_t themeBg     = ST77XX_BLACK;
uint16_t themeText   = ST77XX_WHITE;
uint16_t themeAccent = ST77XX_CYAN;
uint16_t themeSelect = ST77XX_BLUE;
uint16_t themeGreen  = ST77XX_GREEN;
uint16_t themeRed    = ST77XX_RED;
uint16_t themeYellow = ST77XX_YELLOW;
uint16_t themeGray   = 0x8410;

#define COLOR_BG       themeBg
#define COLOR_TEXT     themeText
#define COLOR_ACCENT   themeAccent
#define COLOR_SELECT   themeSelect
#define COLOR_GREEN    themeGreen
#define COLOR_RED      themeRed
#define COLOR_YELLOW   themeYellow
#define COLOR_GRAY     themeGray


Preferences preferences;


enum Screen {
  SCREEN_DESKTOP,
  SCREEN_MENU,
  SCREEN_CALCULATIONS_MENU,
  SCREEN_CALCULATOR,
  SCREEN_PROGRAMMER_CALC,
  SCREEN_PROGRAMMER_OPS,
  SCREEN_TRUTH_TABLE,
  SCREEN_MATH_CALC,
  SCREEN_MATH_FUNCTIONS,
  SCREEN_FUNCTION_GRAPH,
  SCREEN_PASSWORD_GENERATOR,
  SCREEN_HASH_CALC,
  SCREEN_PROGRAM_MENU,
  SCREEN_PROGRAM_RUN,
  SCREEN_PROGRAM_HELP,
  SCREEN_DATETIME_MENU,
  SCREEN_CLOCK,
  SCREEN_TIMER,
  SCREEN_STOPWATCH,
  SCREEN_CALENDAR,
  SCREEN_CALENDAR_EVENT,
  SCREEN_WIFI_MENU,
  SCREEN_WIFI,
  SCREEN_WIFI_PASSWORD,
  SCREEN_RECORDS_MENU,
  SCREEN_NOTES,
  SCREEN_SECURE_PIN,
  SCREEN_SECURE_NOTES,
  SCREEN_CONTACTS,
  SCREEN_CONTACT_VIEW,
  SCREEN_CONTACT_EDIT,
  SCREEN_EDITOR,
  SCREEN_FILES_MENU,
  SCREEN_FILES,
  SCREEN_FILE_OPERATIONS,
  SCREEN_FILE_NAME_INPUT,
  SCREEN_FOLDER_PICKER,
  SCREEN_FILE_INFO,
  SCREEN_TEXT_PICKER,
  SCREEN_TEXT_READER,
  SCREEN_HEX_PICKER,
  SCREEN_HEX_VIEWER,
  SCREEN_IMAGE_PICKER,
  SCREEN_IMAGE_VIEWER,
  SCREEN_SYSTEM,
  SCREEN_HTTP_SHARE,
  SCREEN_HTTP_PIN,
  SCREEN_GAMES_MENU,
  SCREEN_PONG,
  SCREEN_SUDOKU,
  SCREEN_SETTINGS,
  SCREEN_LANGUAGE,
  SCREEN_THEME,
  SCREEN_DESKTOP_SETTINGS,
  SCREEN_WALLPAPER_PICKER,
  SCREEN_LOCK_SETTINGS,
  SCREEN_LOCK_PIN_CHANGE,
  SCREEN_LOCK
};
Screen currentScreen = SCREEN_DESKTOP;


enum SystemLanguage { LANG_RU, LANG_EN };
SystemLanguage systemLanguage = LANG_RU;

const int MENU_ITEMS = 8;
const int MENU_VISIBLE = 5;
const char* menuItemsRu[MENU_ITEMS] = {
  "WI-FI", "ДАТА И ВРЕМЯ", "ВЫЧИСЛЕНИЯ", "ЗАПИСИ",
  "ФАЙЛЫ", "ИГРЫ", "PROGRAM", "НАСТРОЙКИ"
};
const char* menuItemsEn[MENU_ITEMS] = {
  "WI-FI", "DATE & TIME", "CALCULATIONS", "NOTES",
  "FILES", "GAMES", "PROGRAM", "SETTINGS"
};
int selectedMenuItem = 0;
int menuTop = 0;


const int CALC_MENU_ITEMS = 7;
const int CALC_MENU_VISIBLE = 5;
const char* calcMenuItemsRu[CALC_MENU_ITEMS] = {
  "ОБЫЧНЫЙ КАЛЬКУЛЯТОР",
  "ПРОГРАММИСТСКИЙ + КОНВ.",
  "ТАБЛИЦА ИСТИННОСТИ",
  "МАТЕМАТИЧЕСКИЙ КАЛЬК.",
  "ГРАФИК ФУНКЦИЙ",
  "ГЕНЕРАТОР ПАРОЛЕЙ",
  "HASH-КАЛЬКУЛЯТОР"
};
const char* calcMenuItemsEn[CALC_MENU_ITEMS] = {
  "STANDARD CALCULATOR",
  "PROGRAMMER + CONVERTER",
  "TRUTH TABLE",
  "SCIENTIFIC CALC",
  "FUNCTION GRAPH",
  "PASSWORD GENERATOR",
  "HASH CALCULATOR"
};
int calcMenuSelected = 0;
int calcMenuTop = 0;


enum ProgrammerOperation {
  PROG_OP_NONE,
  PROG_OP_AND,
  PROG_OP_NAND,
  PROG_OP_OR,
  PROG_OP_NOR,
  PROG_OP_XOR,
  PROG_OP_XNOR,
  PROG_OP_NOT,
  PROG_OP_BUFFER
};
String progInput = "";
uint16_t progValue = 0;
uint16_t progFirst = 0;
ProgrammerOperation progOperation = PROG_OP_NONE;
bool progWaitingSecond = false;
bool progResultShown = false;
int progOpSelected = 0;
int progOpTop = 0;
const int PROG_OP_COUNT = 8;
const int PROG_OP_VISIBLE = 5;


int truthOpSelected = 0;


String mathInput = "";
double mathValue = 0.0;
bool mathResultShown = false;
bool mathDegrees = true;
int mathFunctionSelected = 0;
int mathFunctionTop = 0;
const int MATH_FUNCTION_COUNT = 13;
const int MATH_FUNCTION_VISIBLE = 5;



const int GRAPH_FUNCTION_COUNT = 10;
const int GRAPH_ZOOM_COUNT = 5;
const float graphHalfRanges[GRAPH_ZOOM_COUNT] = {2.0f, 3.14159265f, 5.0f, 10.0f, 20.0f};
int graphFunctionSelected = 0;
int graphZoomIndex = 2;
bool graphGridEnabled = true;


enum PasswordPreset {
  PASS_ALNUM = 0,
  PASS_STRONG,
  PASS_DIGITS,
  PASS_HEX,
  PASS_PRESET_COUNT
};
const uint8_t passwordLengthOptions[] = {8, 12, 16, 20, 24, 32};
const int PASSWORD_LENGTH_COUNT = sizeof(passwordLengthOptions) / sizeof(passwordLengthOptions[0]);
int passwordPreset = PASS_STRONG;
int passwordLengthIndex = 2;
String generatedPassword = "";


const int DATETIME_ITEMS = 4;
const char* dateTimeItemsRu[DATETIME_ITEMS] = { "ЧАСЫ И ДАТА", "ТАЙМЕР", "СЕКУНДОМЕР", "КАЛЕНДАРЬ" };
const char* dateTimeItemsEn[DATETIME_ITEMS] = { "CLOCK & DATE", "TIMER", "STOPWATCH", "CALENDAR" };
int dateTimeSelected = 0;


int calendarYear = 2026;
int calendarMonth = 1;
int calendarSelectedDay = 1;


const int WIFI_MENU_ITEMS = 2;
const char* wifiMenuItemsRu[WIFI_MENU_ITEMS] = { "СЕТИ / ПОДКЛЮЧЕНИЕ", "HTTP ОБМЕН ФАЙЛАМИ" };
const char* wifiMenuItemsEn[WIFI_MENU_ITEMS] = { "NETWORKS / CONNECT", "HTTP FILE SHARE" };
int wifiMenuSelected = 0;


String calcInput = "";
double calcValue = 0;
char calcOperator = 0;
bool calcHasFirstValue = false;
bool calcResultShown = false;


String oldTimeString = "        ";
String oldDateString = "          ";
unsigned long lastClockUpdate = 0;
bool ntpConfigured = false;



String timerInputDigits = "";
unsigned long timerRemainingMs = 0;
unsigned long timerEndMs = 0;
bool timerRunning = false;
bool timerDone = false;
String oldTimerString = "        ";
String oldTimerStatus = "";
unsigned long lastTimerUiUpdate = 0;


unsigned long stopwatchAccumMs = 0;
unsigned long stopwatchStartMs = 0;
bool stopwatchRunning = false;
String oldStopwatchString = "       ";
unsigned long lastStopwatchUiUpdate = 0;


int wifiNetworkCount = 0;
int wifiSelected = 0;
int wifiTop = 0;
const int WIFI_VISIBLE_ROWS = 4;
String selectedSSID = "";
int selectedAuth = WIFI_AUTH_OPEN;
String wifiPassword = "";
unsigned long lastReconnectAttempt = 0;


enum InputMode {
  INPUT_LAT_LOWER,
  INPUT_LAT_UPPER,
  INPUT_RUS_LOWER,
  INPUT_RUS_UPPER,
  INPUT_NUMBERS,
  INPUT_NAV
};
InputMode passwordMode = INPUT_LAT_LOWER;
int passwordCursor = 0;
InputMode editorMode = INPUT_LAT_LOWER;


const int MAX_HASH_INPUT_BYTES = 512;
String hashInput = "";
int hashCursor = 0;
InputMode hashInputMode = INPUT_LAT_LOWER;
char hashLastKey = 0;
int hashMultiIndex = 0;
unsigned long hashLastTime = 0;
bool hashMultiActive = false;
bool hashShowingResult = false;
String hashSha1 = "";
String hashSha256 = "";
String hashCrc32 = "";


const int MAX_CALENDAR_EVENT_BYTES = 512;
String calendarEventText = "";
int calendarEventCursor = 0;
InputMode calendarEventMode = INPUT_RUS_LOWER;
char calendarEventLastKey = 0;
int calendarEventMultiIndex = 0;
unsigned long calendarEventLastTime = 0;
bool calendarEventMultiActive = false;
bool calendarEventDirty = false;


char passwordLastKey = 0;
int passwordMultiIndex = 0;
unsigned long passwordLastTime = 0;
bool passwordMultiActive = false;


const int RECORDS_MENU_ITEMS = 3;
int recordsMenuSelected = 0;


const int MAX_SECURE_NOTES = 99;
const int SECURE_VISIBLE_ROWS = 4;
String secureNoteFiles[MAX_SECURE_NOTES];
int secureNoteCount = 0;
int secureNoteSelected = 0;
int secureNoteTop = 0;
bool secureDeleteArmed = false;

String securePinHash = "";
String securePinInput = "";
String securePinFirst = "";
String secureSessionPin = "";
bool secureVaultUnlocked = false;

enum SecureVaultTarget { VAULT_TARGET_NOTES, VAULT_TARGET_CONTACTS };
SecureVaultTarget secureVaultTarget = VAULT_TARGET_NOTES;

uint8_t securePinStage = 0;
bool secureEditorMode = false;





const int MAX_CONTACTS = 50;
const int CONTACT_VISIBLE_ROWS = 4;
String contactPaths[MAX_CONTACTS];
String contactNames[MAX_CONTACTS];
int contactCount = 0;
int contactSelected = 0;
int contactTop = 0;
bool contactDeleteArmed = false;

String contactEditPath = "";
String contactName = "";
String contactPhone = "";
String contactEmail = "";
String contactAddress = "";
int contactCursor = 0;
int contactEditField = 0;
InputMode contactInputMode = INPUT_LAT_LOWER;
char contactLastKey = 0;
int contactMultiIndex = 0;
unsigned long contactLastTime = 0;
bool contactMultiActive = false;
bool contactEditingExisting = false;


const int MAX_NOTES = 99;
const int MAX_NOTE_BYTES = 8192;
const int NOTE_VISIBLE_ROWS = 4;
String noteFiles[MAX_NOTES];
int noteCount = 0;
int noteSelected = 0;
int noteTop = 0;
bool noteDeleteArmed = false;


String currentNotePath = "";
String noteText = "";

int editCursor = 0;
bool noteDirty = false;
const int EDIT_COLS = 16;
const int EDIT_ROWS = 7;
const int EDIT_TEXT_WIDTH = 100;
const int EDIT_HELP_X = 103;
int editorTopLine = 0;
char editorLastKey = 0;
int editorMultiIndex = 0;
unsigned long editorLastTime = 0;
bool editorMultiActive = false;
Screen editorReturnScreen = SCREEN_NOTES;




const char* PROGRAM_FILE = "/PROGRAM.PPC";
const int PROGRAM_MENU_ITEMS = 3;
const int PROGRAM_OUTPUT_LINES = 7;
int programMenuSelected = 0;
int programHelpPage = 0;
String programSource = "";
int32_t programRegs[26] = {0};
int programPc = 0;
int programLineCount = 0;
bool programRunning = false;
bool programPaused = false;
bool programFinished = false;
String programStatus = "";
String programOutput[PROGRAM_OUTPUT_LINES];
int programOutputCount = 0;
unsigned long programNextStepAt = 0;
unsigned long programExecutedSteps = 0;




bool programStatusUiInitialized = false;
String programLastStateDrawn = "";
int programLastLineDrawn = -1;
bool programLastControlRunning = false;
bool programLastControlValid = false;
unsigned long programLastLineUiAt = 0;


const int MAX_FILES = 64;
const int FILE_VISIBLE_ROWS = 4;
String fileNames[MAX_FILES];
size_t fileSizes[MAX_FILES];
int fileCount = 0;
int fileSelected = 0;
int fileTop = 0;
bool fileDeleteArmed = false;




const int FM_MAX_ITEMS = 64;
String fmCurrentDir = "/";
String fmPaths[FM_MAX_ITEMS];
String fmNames[FM_MAX_ITEMS];
size_t fmSizes[FM_MAX_ITEMS];
bool fmIsDir[FM_MAX_ITEMS];
int fmCount = 0;
int fmSelected = 0;
int fmTop = 0;
bool fmDeleteArmed = false;

const int FILE_OP_ITEMS = 6;
int fileOpsSelected = 0;
bool fileOpsDeleteArmed = false;
enum FileNameAction {
  FNAME_NONE,
  FNAME_RENAME,
  FNAME_NEW_FOLDER
};
FileNameAction fileNameAction = FNAME_NONE;
String fileOpSourcePath = "";
String fileNameInput = "";
int fileNameCursor = 0;
InputMode fileNameInputMode = INPUT_LAT_LOWER;
char fileNameLastKey = 0;
int fileNameMultiIndex = 0;
unsigned long fileNameLastTime = 0;
bool fileNameMultiActive = false;
const int MAX_FILE_NAME_BYTES = 48;


String folderPickerDir = "/";
String folderPaths[FM_MAX_ITEMS];
String folderNames[FM_MAX_ITEMS];
int folderCount = 0;
int folderSelected = 0;
int folderTop = 0;
bool folderPickerMove = false;


const int FILES_MENU_ITEMS = 3;
const char* filesMenuItemsRu[FILES_MENU_ITEMS] = { "МЕНЕДЖЕР ФАЙЛОВ", "ЧТЕНИЕ ТЕКСТА", "HEX-РЕДАКТОР" };
const char* filesMenuItemsEn[FILES_MENU_ITEMS] = { "FILE MANAGER", "TEXT READER", "HEX EDITOR" };
int filesMenuSelected = 0;


String readerPath = "";
String readerText = "";
int readerTopLine = 0;
const int READER_COLS = 25;
const int READER_ROWS = 7;


String hexPath = "";
size_t hexOffset = 0;
size_t hexCursor = 0;
bool hexEditMode = false;
bool hexAsciiMode = false;
String hexEditDigits = "";
String hexStatus = "";
unsigned long hexStatusUntil = 0;
const int HEX_BYTES_PER_ROW = 6;
const int HEX_ROWS = 10;


String imagePath = "";
int32_t imageWidth = 0;
int32_t imageHeight = 0;
uint32_t imageDataOffset = 0;
uint16_t imageBitDepth = 0;
uint32_t imageCompression = 0;
bool imageTopDown = false;
int imagePanX = 0;
int imagePanY = 0;
bool imageDeleteArmed = false;
const int IMAGE_VIEW_Y = 18;
const int IMAGE_VIEW_H = 96;


int systemPage = 0;
const int SYSTEM_PAGES = 2;


int settingsSelected = 0;
int languageSelected = 0;
int themeSelected = THEME_CLASSIC;





bool lockEnabled = false;
uint32_t lockTimeoutMinutes = 5;
String systemPin = "1234";
bool systemLocked = false;
unsigned long lastUserActivity = 0;
String unlockPinInput = "";
String lockPinEdit = "";
int lockSettingsSelected = 0;
Screen unlockTargetScreen = SCREEN_MENU;

const uint8_t LOCK_TIMEOUT_COUNT = 5;
const uint32_t lockTimeoutOptions[LOCK_TIMEOUT_COUNT] = {1, 2, 5, 10, 30};




String wallpaperPath = "";
bool desktopShowClock = true;
bool desktopShowDate = true;
bool desktopShowHint = true;


bool desktopShowReminders = true;
String desktopReminderDateKey = "";
String desktopOldTime = "        ";
String desktopOldDate = "          ";
unsigned long desktopLastUpdate = 0;
int desktopSettingsSelected = 0;

const int MAX_WALLPAPERS = MAX_FILES + 1;
String wallpaperFiles[MAX_WALLPAPERS];
int wallpaperCount = 0;
int wallpaperSelected = 0;
int wallpaperTop = 0;
const int WALLPAPER_VISIBLE_ROWS = 5;






WebServer httpServer(80);
File httpUploadFile;

bool httpRoutesConfigured = false;
bool httpServerRunning = false;
bool httpUploadFailed = false;
String httpUploadPath = "";
size_t httpUploadBytes = 0;
unsigned long httpRequestCount = 0;
String httpPin = "1234";
String httpPinEdit = "";


const int GAMES_MENU_ITEMS = 2;
const char* gamesMenuItemsRu[GAMES_MENU_ITEMS] = { "ПИНГ-ПОНГ", "СУДОКУ 4x4" };
const char* gamesMenuItemsEn[GAMES_MENU_ITEMS] = { "PING PONG", "SUDOKU 4x4" };
int gamesMenuSelected = 0;





uint8_t sudokuBoard[16] = {0};
bool sudokuFixed[16] = {false};
uint8_t sudokuSolution[16] = {0};
int sudokuCursor = 0;
bool sudokuSolved = false;
int sudokuPuzzleIndex = 0;




const int PONG_FIELD_TOP = 20;
const int PONG_FIELD_BOTTOM = 106;
const int PONG_PLAYER_X = 5;
const int PONG_AI_X = 152;
const int PONG_PADDLE_W = 3;
const int PONG_PADDLE_H = 22;
const int PONG_BALL_SIZE = 4;
const int PONG_WIN_SCORE = 5;

int pongPlayerY = 52;
int pongAiY = 52;
int pongBallX = 78;
int pongBallY = 60;
int pongBallVX = 2;
int pongBallVY = 1;
int pongPlayerScore = 0;
int pongAiScore = 0;
bool pongPaused = false;
bool pongGameOver = false;
unsigned long pongLastFrame = 0;
const unsigned long PONG_FRAME_MS = 28;




void printCentered(const String& text, int y, uint16_t color, int size);
void showMessage(const String& line1, const String& line2, uint16_t color, int ms = 900);
void bootScreen();

void drawMenu();
void menuKey(char key);
void openSelectedMenuItem();
void drawCalculationsMenu();
void calculationsMenuKey(char key);
String calcMenuItemText(int i);

void drawProgrammerCalculator();
void programmerCalculatorKey(char key);
void drawProgrammerOperations();
void programmerOperationsKey(char key);
String programmerOperationName(int index);
String programmerOperationDescription(int index);
void programmerApplyOperation(int index);
void programmerUpdateDisplay();
uint16_t programmerCurrentValue();
String binary16(uint16_t value);
void drawTruthTable();
void truthTableKey(char key);
int truthGateResult(int opIndex, int a, int b = 0);

void drawMathCalculator();
void mathCalculatorKey(char key);
void drawMathFunctions();
void mathFunctionsKey(char key);
String mathFunctionName(int index);
String mathFunctionDescription(int index);
void mathApplyFunction(int index);
void mathUpdateDisplay();
String formatMathNumber(double value);

void drawPasswordGenerator();
void passwordGeneratorKey(char key);
String passwordPresetName(int preset);
void generatePassword();

void drawHashCalculator();
void hashCalculatorKey(char key);
void hashMultiTap(char key);
void drawHashInput();
void drawHashHelp();
String crc32Hex(const uint8_t* data, size_t len);
String sha1Hex(const uint8_t* data, size_t len);
String sha256Hex(const uint8_t* data, size_t len);

void drawProgramMenu();
void programMenuKey(char key);
void ensureProgramFile();
void openProgramEditor();
void startProgram();
void drawProgramRun();
void updateProgramRun();
void programRunKey(char key);
void drawProgramHelp();
void programHelpKey(char key);
String programGetLine(int index);
int programCountLines(const String& source);
String programToken(const String& s, int tokenIndex);
String programRestAfterFirstToken(const String& s);
bool programRegisterIndex(const String& token, int& index);
int32_t programResolveValue(const String& token, bool& ok);
void programPushOutput(const String& line);
void programDrawOutput();
void programDrawStatus();
void programFail(int lineNumber, const String& message);
void executeProgramLine(const String& line, int sourceLineNumber);

void drawDateTimeMenu();
void dateTimeMenuKey(char key);
void drawTimerScreen();
void timerKey(char key);
void updateTimer(bool forceUpdate = false);
String formatTimerMs(unsigned long ms);
unsigned long timerDigitsToMs(const String& digits);
void drawStopwatchScreen();
void stopwatchKey(char key);
void updateStopwatch(bool forceUpdate = false);
String formatStopwatchMs(unsigned long ms);
void initCalendarToToday();
void drawCalendar();
void calendarKey(char key);
int calendarDaysInMonth(int year, int month);
int calendarWeekdayMon0(int year, int month, int day);
String calendarMonthName(int month);
void calendarMoveDays(int delta);
void calendarMoveMonth(int delta);
String calendarEventPath(int year, int month, int day);
bool calendarHasEvent(int year, int month, int day);
String loadCalendarEvent(int year, int month, int day);
bool saveCalendarEvent(int year, int month, int day, const String& text);
void openCalendarEventEditor();
void drawCalendarEventEditor();
void calendarEventKey(char key);
void calendarEventMultiTap(char key);
void drawCalendarEventText();
void drawCalendarEventHelp();
String calendarEventModeName();
void drawWiFiMenu();
void wifiMenuKey(char key);

void drawCalculator();
void updateCalculatorDisplay();
void calculatorKey(char key);
double calculateResult(double a, double b, char op);
String formatNumber(double value);

void drawClockScreen();
void updateClock(bool forceUpdate = false);
void drawChangedString(String newValue, String& oldValue, int x, int y, int size, uint16_t color);
void configureKaliningradTime();

void connectSavedWiFi();
void scanWiFiNetworks();
void drawWiFiScreen();
void drawWiFiRows();
void wifiKey(char key);
void selectWiFiNetwork();
void drawPasswordScreen();
void updatePasswordField();
void passwordKey(char key);
void passwordMultiTap(char key);
void connectToWiFi(String ssid, String password);

String inputModeName(InputMode mode);
String getCharacterSet(char key, InputMode mode);

int utf8SeqLen(uint8_t lead);
int utf8NextIndex(const String& s, int byteIndex);
int utf8PrevIndex(const String& s, int byteIndex);
String utf8GlyphAt(const String& s, int byteIndex);
String utf8GlyphAtNumber(const String& s, int glyphNumber);
int utf8Count(const String& s);
String utf8Truncate(const String& s, int maxGlyphs);
void utf8InsertAt(String& s, int byteIndex, const String& glyph);
void utf8ReplacePrevious(String& s, int& cursor, const String& glyph);

void drawRecordsMenu();
void recordsMenuKey(char key);

void refreshContactList();
void drawContactsScreen();
void drawContactRows();
void contactsKey(char key);
bool loadContact(const String& path, String& name, String& phone, String& email, String& address);
bool saveContact(const String& path, const String& name, const String& phone, const String& email, const String& address);
bool loadLegacyPlainContact(const String& path, String& name, String& phone, String& email, String& address);
String serializeContact(const String& name, const String& phone, const String& email, const String& address);
bool parseContactPayload(const String& plain, String& name, String& phone, String& email, String& address);
bool contactFileHasSecureHeader(const String& path);
String createContactPath();
void openSelectedContact();
void startNewContact();
void startEditContact();
void drawContactView();
void contactViewKey(char key);
void drawContactEditor();
void drawContactEditorText();
void drawContactEditorHelp();
void contactEditorKey(char key);
void contactMultiTap(char key);
String* contactActiveField();
int contactActiveMaxBytes();
String contactFieldName(int field);
void deleteSelectedContact();
String contactStripLineEnd(String v);
void drawContactWrapped(const String& value, int x, int baseline, int cols, int maxLines, uint16_t color);
void contactSetRecommendedMode();

void beginSecureVault(SecureVaultTarget target);
void openSecureVaultTarget();
void drawSecurePinScreen();
void securePinKey(char key);
String securePinDigest(const String& pin);
void secureDeriveKey(const String& pin, const uint8_t salt[16], uint8_t key[32]);
bool secureEncryptFile(const String& path, const String& plain, const String& pin);
bool secureDecryptFile(const String& path, String& plain, const String& pin);
void refreshSecureNoteList();
void drawSecureNotesScreen();
void drawSecureNoteRows();
void secureNotesKey(char key);
String createNewSecureNote();
void openSecureNote(const String& path);
void deleteSelectedSecureNote();
void closeSecureVault();

void refreshNoteList();
void drawNotesScreen();
void drawNoteRows();
void notesKey(char key);
String createNewNote();
void openNote(String path);
void deleteSelectedNote();
String getNotePreview(String path);

void drawEditor();
void drawEditorHeader();
void drawEditorBody();
void drawEditorHelp();
void drawEditorFooter();
void editorKey(char key);
void editorMultiTap(char key);
void insertEditorGlyph(const String& glyph);
void backspaceEditor();
void saveCurrentNote();
void getVisualPosition(int byteIndex, int& line, int& column);
int indexForVisualPosition(int targetLine, int targetColumn);
void moveCursorHorizontal(int delta);
void moveCursorVertical(int delta);
void drawUtf8(int x, int baseline, const String& text, uint16_t color, const uint8_t* font);
void uiText(int x, int baseline, const String& text, uint16_t color = COLOR_TEXT);
void uiTextSmall(int x, int baseline, const String& text, uint16_t color = COLOR_TEXT);
String T(const char* ru, const char* en);
void drawUiCentered(const String& text, int baseline, uint16_t color, const uint8_t* font, int glyphWidth);
String menuItemText(int i);
String calcMenuItemText(int i);
String dateTimeItemText(int i);
String wifiMenuItemText(int i);
String filesMenuItemText(int i);
void drawSettings();
void settingsKey(char key);
void drawLanguageSettings();
void languageKey(char key);
void applyTheme(uint8_t theme);
String themeName(uint8_t theme);
void drawThemeSettings();
void themeKey(char key);
void drawLockSettings();
void lockSettingsKey(char key);
void drawLockPinChange();
void lockPinChangeKey(char key);
void drawLockScreen();
void lockKey(char key);
void beginUnlock(Screen target);
void completeUnlock();
void updateAutoLock();
void markUserActivity();
uint8_t currentLockTimeoutIndex();

void drawDesktop();
void updateDesktop(bool forceUpdate = false);
void drawDesktopReminders();
bool getCurrentDate(int& year, int& month, int& day);
void addDaysToDate(int year, int month, int day, int delta, int& outYear, int& outMonth, int& outDay);
String desktopReminderEvent(int dayOffset);
void desktopKey(char key);
void goHome();
bool drawDesktopWallpaper(const String& path);
void drawDesktopSettings();
void desktopSettingsKey(char key);
void refreshWallpaperList();
void drawWallpaperPicker();
void wallpaperPickerKey(char key);
void applyWallpaper(const String& path);

void drawFunctionGraph();
void functionGraphKey(char key);
double graphEvaluate(int index, double x, bool& valid);
String graphFunctionName(int index);
String graphCurveName(int index);
void graphYBounds(int index, double xHalf, double& yMin, double& yMax);

void drawFilesMenu();
void filesMenuKey(char key);
void refreshFileList();
void drawFilePicker(const String& title, bool textOnly);
void filePickerKey(char key, bool textOnly);
bool isTextFile(const String& path);
void openTextReader(const String& path);
void drawTextReader();
void textReaderKey(char key);
int readerTotalLines();
void openHexViewer(const String& path);
void drawHexViewer();
void hexViewerKey(char key);
int hexNibbleValue(char key);
bool writeHexByte(size_t offset, uint8_t value);

bool isImageFile(const String& path);
void drawImagePicker();
void imagePickerKey(char key);
void openImageViewer(const String& path);
void drawImageViewer();
void imageViewerKey(char key);
bool openAdjacentImage(int direction);
bool deleteCurrentImage();
int findImageFileIndex(const String& path);
bool loadBmpInfo(File& f);
uint16_t read16le(File& f);
uint32_t read32le(File& f);
void drawFilesScreen();
void drawFileRows();
void filesKey(char key);
void deleteSelectedFile();
void openSelectedFile();
String formatBytes(size_t bytes);

void refreshFmList();
String fmBaseName(const String& path);
String fmParentDir(const String& path);
String fmJoinPath(const String& dir, const String& name);
bool fmProtectedPath(const String& path);
bool fmDeleteRecursive(const String& path);
bool fmCopyRecursive(const String& source, const String& destination);
String fmUniqueDestination(const String& dir, const String& name, bool isDir);
void fmUpdateWallpaperPath(const String& oldPath, const String& newPath);
void drawFileOperations();
void fileOperationsKey(char key);
void beginFileNameInput(FileNameAction action);
void drawFileNameInput();
void fileNameInputKey(char key);
void fileNameMultiTap(char key);
bool validFileName(String name);
void refreshFolderPicker();
void drawFolderPicker();
void folderPickerKey(char key);
void executeCopyMoveTo(const String& destinationDir);
void drawFileInfo();
void fileInfoKey(char key);
String fmExtension(const String& path);

void drawSystemInfo();
void systemKey(char key);
String uptimeString();

void configureHttpRoutes();
void startHttpServer();
void stopHttpServer();
void drawHttpShare();
void httpShareKey(char key);
void drawHttpPinScreen();
void httpPinKey(char key);
bool httpIsAuthorized();
void httpSendLoginPage(bool wrongPin);
void httpHandleIndex();
void httpHandleDownload();
void httpHandleUploadData();
void httpHandleUploadDone();
void httpHandleNotFound();
String httpHtmlEscape(const String& text);
String httpUrlEncode(const String& text);
String httpSafeBaseName(String name);
String httpUniqueUploadPath(const String& baseName);
String httpContentType(const String& path);

void drawGamesMenu();
void gamesMenuKey(char key);
String gamesMenuItemText(int i);

void sudokuLoadPuzzle(int index);
void sudokuNewGame();
void drawSudoku();
void sudokuKey(char key);
bool sudokuIsComplete();
bool sudokuMatchesSolution();
bool sudokuCellConflicts(int cell);

void drawPong();
void drawPongScore();
void pongKey(char key);
void updatePong();
void resetPongGame();
void resetPongBall(int direction);
void drawPongStatus(const String& text, uint16_t color);

void handlePressedKey(char key);
void handleHeldKey(char key);




void printCentered(const String& text, int y, uint16_t color, int size) {
  tft.setTextSize(size);
  tft.setTextColor(color);
  int width = text.length() * 6 * size;
  int x = (tft.width() - width) / 2;
  if (x < 0) x = 0;
  tft.setCursor(x, y);
  tft.print(text);
}

void showMessage(const String& line1, const String& line2, uint16_t color, int ms) {
  tft.fillScreen(COLOR_BG);
  int w1 = utf8Count(line1) * 5;
  int x1 = (160 - w1) / 2; if (x1 < 2) x1 = 2;
  uiText(x1, 51, line1, color);
  if (line2.length()) {
    int w2 = utf8Count(line2) * 5;
    int x2 = (160 - w2) / 2; if (x2 < 2) x2 = 2;
    uiText(x2, 67, line2, color);
  }
  delay(ms);
}

void drawUtf8(int x, int baseline, const String& text, uint16_t color, const uint8_t* font) {
  u8g2.setFont(font);
  u8g2.setFontMode(1);
  u8g2.setForegroundColor(color);
  u8g2.setBackgroundColor(COLOR_BG);
  u8g2.setCursor(x, baseline);
  u8g2.print(text);
}

void uiText(int x, int baseline, const String& text, uint16_t color) {

  drawUtf8(x, baseline, text, color, u8g2_font_6x12_t_cyrillic);
}

void uiTextSmall(int x, int baseline, const String& text, uint16_t color) {

  drawUtf8(x, baseline, text, color, u8g2_font_5x7_t_cyrillic);
}

String T(const char* ru, const char* en) {
  return systemLanguage == LANG_RU ? String(ru) : String(en);
}

String menuItemText(int i) { return String(systemLanguage == LANG_RU ? menuItemsRu[i] : menuItemsEn[i]); }
String calcMenuItemText(int i) { return String(systemLanguage == LANG_RU ? calcMenuItemsRu[i] : calcMenuItemsEn[i]); }
String dateTimeItemText(int i) { return String(systemLanguage == LANG_RU ? dateTimeItemsRu[i] : dateTimeItemsEn[i]); }
String wifiMenuItemText(int i) { return String(systemLanguage == LANG_RU ? wifiMenuItemsRu[i] : wifiMenuItemsEn[i]); }
String filesMenuItemText(int i) { return String(systemLanguage == LANG_RU ? filesMenuItemsRu[i] : filesMenuItemsEn[i]); }

void drawUiCentered(const String& text, int baseline, uint16_t color, const uint8_t* font, int glyphWidth) {
  int width = utf8Count(text) * glyphWidth;
  int x = (160 - width) / 2;
  if (x < 2) x = 2;
  drawUtf8(x, baseline, text, color, font);
}




void bootScreen() {
  tft.fillScreen(COLOR_BG);
  tft.drawRect(0, 0, tft.width(), tft.height(), COLOR_ACCENT);
  printCentered("PPC", 18, COLOR_ACCENT, 3);
  printCentered("OS", 47, COLOR_TEXT, 2);



  drawUiCentered(T("Портативный компьютер", "Portable computer"), 80, COLOR_GRAY, u8g2_font_5x7_t_cyrillic, 5);
  drawUiCentered(T("Версия 2.8.5", "Version 2.8.5"), 94, COLOR_GRAY, u8g2_font_5x7_t_cyrillic, 5);

  int x = 25, y = 108, w = 110, h = 7;
  tft.drawRect(x, y, w, h, COLOR_GRAY);
  for (int i = 1; i < w - 2; i += 3) {
    tft.fillRect(x + 1, y + 1, i, h - 2, COLOR_ACCENT);
    delay(15);
  }
  delay(220);
}






bool drawDesktopWallpaper(const String& path) {
  if (path.length() == 0 || !LittleFS.exists(path)) return false;

  File f = LittleFS.open(path, "r");
  if (!f || !loadBmpInfo(f)) {
    if (f) f.close();
    return false;
  }

  tft.fillScreen(COLOR_BG);

  int bytesPerPixel = imageBitDepth / 8;
  uint32_t rowSize = ((uint32_t)imageWidth * bytesPerPixel + 3) & ~3UL;

  int drawW = min(160, (int)imageWidth);
  int drawH = min(128, (int)imageHeight);
  int srcX = max(0, ((int)imageWidth - drawW) / 2);
  int srcY = max(0, ((int)imageHeight - drawH) / 2);
  int destX = max(0, (160 - drawW) / 2);
  int destY = max(0, (128 - drawH) / 2);

  static uint8_t raw[160 * 4];
  static uint16_t line565[160];

  for (int dy = 0; dy < drawH; dy++) {
    int srcYTop = srcY + dy;
    int srcRow = imageTopDown ? srcYTop : (imageHeight - 1 - srcYTop);
    uint32_t pos = imageDataOffset + (uint32_t)srcRow * rowSize + (uint32_t)srcX * bytesPerPixel;
    if (!f.seek(pos)) break;
    size_t need = drawW * bytesPerPixel;
    if (f.read(raw, need) < need) break;

    for (int x = 0; x < drawW; x++) {
      uint8_t b = raw[x * bytesPerPixel + 0];
      uint8_t g = raw[x * bytesPerPixel + 1];
      uint8_t r = raw[x * bytesPerPixel + 2];
      line565[x] = tft.color565(r, g, b);
    }
    tft.drawRGBBitmap(destX, destY + dy, line565, drawW, 1);
  }

  f.close();
  return true;
}

void drawDesktop() {
  bool wallpaperOk = drawDesktopWallpaper(wallpaperPath);
  if (!wallpaperOk) {

    if (wallpaperPath.length()) {
      wallpaperPath = "";
      preferences.putString("wallpaper", "");
    }
    tft.fillScreen(COLOR_BG);


    tft.drawRoundRect(5, 5, 150, 118, 8, COLOR_SELECT);
    tft.drawFastHLine(15, 24, 130, COLOR_GRAY);
  }



  tft.fillRect(0, 0, 160, 17, COLOR_BG);
  tft.drawFastHLine(0, 17, 160, COLOR_GRAY);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT, COLOR_BG);
  tft.setCursor(4, 5);
  tft.print("PPC");
  if (systemLocked) {
    uiTextSmall(108, 12, T("ЗАМОК", "LOCK"), COLOR_YELLOW);
  }
  tft.setCursor(140, 5);
  tft.print(WiFi.status() == WL_CONNECTED ? "W+" : "W-");


  tft.fillRoundRect(18, 54, 124, 49, 6, COLOR_BG);
  tft.drawRoundRect(18, 54, 124, 49, 6, COLOR_ACCENT);

  desktopOldTime = "        ";
  desktopOldDate = "          ";
  desktopReminderDateKey = "";
  updateDesktop(true);

  if (desktopShowHint || systemLocked) {
    tft.fillRect(0, 111, 160, 17, COLOR_BG);
    String hint = systemLocked ? T("A - РАЗБЛОК", "A - UNLOCK") : T("A - МЕНЮ", "A - MENU");
    drawUiCentered(hint, 124, systemLocked ? COLOR_YELLOW : COLOR_GRAY, u8g2_font_5x7_t_cyrillic, 5);
  }
}

bool getCurrentDate(int& year, int& month, int& day) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 15) || timeinfo.tm_year + 1900 < 2020) return false;
  year = timeinfo.tm_year + 1900;
  month = timeinfo.tm_mon + 1;
  day = timeinfo.tm_mday;
  return true;
}

void addDaysToDate(int year, int month, int day, int delta,
                   int& outYear, int& outMonth, int& outDay) {
  outYear = year;
  outMonth = month;
  outDay = day;
  while (delta > 0) {
    outDay++;
    if (outDay > calendarDaysInMonth(outYear, outMonth)) {
      outDay = 1;
      outMonth++;
      if (outMonth > 12) { outMonth = 1; outYear++; }
    }
    delta--;
  }
  while (delta < 0) {
    outDay--;
    if (outDay < 1) {
      outMonth--;
      if (outMonth < 1) { outMonth = 12; outYear--; }
      outDay = calendarDaysInMonth(outYear, outMonth);
    }
    delta++;
  }
}

String desktopReminderEvent(int dayOffset) {
  int y, m, d;
  if (!getCurrentDate(y, m, d)) return "";
  int ry, rm, rd;
  addDaysToDate(y, m, d, dayOffset, ry, rm, rd);
  String text = loadCalendarEvent(ry, rm, rd);
  text.replace("\r", " ");
  text.replace("\n", " ");
  while (text.indexOf("  ") >= 0) text.replace("  ", " ");
  text.trim();
  return text;
}

void drawDesktopReminders() {
  if (!desktopShowReminders) return;

  String tomorrow = desktopReminderEvent(1);
  String afterTomorrow = desktopReminderEvent(2);
  if (!tomorrow.length() && !afterTomorrow.length()) return;



  tft.fillRoundRect(3, 20, 154, 31, 4, COLOR_BG);
  tft.drawRoundRect(3, 20, 154, 31, 4, COLOR_YELLOW);

  int y = 31;
  if (tomorrow.length()) {
    String line = T("ЗАВТРА: ", "TOMORROW: ") + tomorrow;
    drawUtf8(7, y, utf8Truncate(line, 29), COLOR_YELLOW, u8g2_font_5x7_t_cyrillic);
    y += 14;
  }
  if (afterTomorrow.length()) {
    String line = T("ПОСЛЕЗАВТРА: ", "IN 2 DAYS: ") + afterTomorrow;
    drawUtf8(7, y, utf8Truncate(line, 29), COLOR_TEXT, u8g2_font_5x7_t_cyrillic);
  }
}

void updateDesktop(bool forceUpdate) {
  if (!forceUpdate && millis() - desktopLastUpdate < 200) return;
  desktopLastUpdate = millis();

  struct tm timeinfo;
  bool haveTime = getLocalTime(&timeinfo, 10);

  String dateKey = "";
  if (haveTime) {
    char kb[9];
    strftime(kb, sizeof(kb), "%Y%m%d", &timeinfo);
    dateKey = String(kb);




    if (!forceUpdate && desktopShowReminders && desktopReminderDateKey.length() &&
        dateKey != desktopReminderDateKey) {
      drawDesktop();
      return;
    }
    desktopReminderDateKey = dateKey;
  }

  if (desktopShowClock) {
    String timeText = "--:--:--";
    if (haveTime) {
      char tb[9];
      strftime(tb, sizeof(tb), "%H:%M:%S", &timeinfo);
      timeText = String(tb);
    }
    drawChangedString(timeText, desktopOldTime, 32, 63, 2, COLOR_TEXT);
  } else {
    tft.fillRect(28, 61, 104, 18, COLOR_BG);
    desktopOldTime = "        ";
  }

  if (desktopShowDate) {
    String dateText = "--.--.----";
    if (haveTime) {
      char db[11];
      strftime(db, sizeof(db), "%d.%m.%Y", &timeinfo);
      dateText = String(db);
    }
    drawChangedString(dateText, desktopOldDate, 50, 86, 1, COLOR_ACCENT);
  } else {
    tft.fillRect(48, 84, 64, 10, COLOR_BG);
    desktopOldDate = "          ";
  }

  if (forceUpdate) drawDesktopReminders();
}

void markUserActivity() {
  lastUserActivity = millis();
}

uint8_t currentLockTimeoutIndex() {
  for (uint8_t i = 0; i < LOCK_TIMEOUT_COUNT; i++) {
    if (lockTimeoutMinutes == lockTimeoutOptions[i]) return i;
  }
  return 2;
}

void updateAutoLock() {
  if (!lockEnabled || systemLocked || currentScreen != SCREEN_DESKTOP) return;

  unsigned long timeoutMs = lockTimeoutMinutes * 60000UL;
  if (timeoutMs == 0) return;

  if ((unsigned long)(millis() - lastUserActivity) >= timeoutMs) {
    systemLocked = true;
    drawDesktop();
  }
}

void beginUnlock(Screen target) {
  unlockTargetScreen = target;
  unlockPinInput = "";
  currentScreen = SCREEN_LOCK;
  drawLockScreen();
}

void completeUnlock() {
  systemLocked = false;
  markUserActivity();

  if (unlockTargetScreen == SCREEN_DATETIME_MENU) {
    currentScreen = SCREEN_DATETIME_MENU;
    dateTimeSelected = 0;
    drawDateTimeMenu();
  } else {
    currentScreen = SCREEN_MENU;
    drawMenu();
  }
}

void drawLockScreen() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("БЛОКИРОВКА", "LOCKED"), COLOR_TEXT);

  drawUiCentered(T("ВВЕДИТЕ PIN", "ENTER PIN"), 44, COLOR_YELLOW, u8g2_font_6x12_t_cyrillic, 6);

  String masked = "";
  for (int i = 0; i < unlockPinInput.length(); i++) masked += '*';
  if (masked.length() == 0) masked = "_";
  drawUiCentered(masked, 72, COLOR_TEXT, u8g2_font_6x12_t_cyrillic, 6);

  uiTextSmall(8, 95, T("0-9 ВВОД   B УДАЛИТЬ", "0-9 INPUT  B DELETE"), COLOR_GRAY);
  uiTextSmall(8, 107, T("A/# OK   C ОЧИСТИТЬ", "A/# OK    C CLEAR"), COLOR_GRAY);
  uiTextSmall(8, 121, T("D - РАБОЧИЙ СТОЛ", "D - DESKTOP"), COLOR_GRAY);
}

void lockKey(char key) {
  if (key >= '0' && key <= '9') {
    if (unlockPinInput.length() < 6) unlockPinInput += key;
    drawLockScreen();
    return;
  }

  if (key == 'B') {
    if (unlockPinInput.length()) unlockPinInput.remove(unlockPinInput.length() - 1);
    drawLockScreen();
    return;
  }

  if (key == 'C') {
    unlockPinInput = "";
    drawLockScreen();
    return;
  }

  if (key == 'A' || key == '#') {
    if (unlockPinInput == systemPin) {
      completeUnlock();
    } else {
      showMessage(T("НЕВЕРНЫЙ PIN", "WRONG PIN"), T("ПОПРОБУЙТЕ ЕЩЁ", "TRY AGAIN"), COLOR_RED, 650);
      unlockPinInput = "";
      drawLockScreen();
    }
    return;
  }

  if (key == 'D') {
    currentScreen = SCREEN_DESKTOP;
    drawDesktop();
  }
}

void desktopKey(char key) {
  if (key == 'A') {
    if (systemLocked) beginUnlock(SCREEN_MENU);
    else {
      currentScreen = SCREEN_MENU;
      drawMenu();
    }
    return;
  }


  if (key == '#') {
    if (systemLocked) beginUnlock(SCREEN_DATETIME_MENU);
    else {
      currentScreen = SCREEN_DATETIME_MENU;
      dateTimeSelected = 0;
      drawDateTimeMenu();
    }
    return;
  }


  if (key == 'D') drawDesktop();
}

void goHome() {
  if (secureEditorMode && noteDirty) saveCurrentNote();
  if (secureEditorMode) noteText = "";
  if (secureVaultUnlocked || currentScreen == SCREEN_SECURE_PIN || currentScreen == SCREEN_SECURE_NOTES || secureEditorMode) {
    closeSecureVault();
  }
  currentScreen = SCREEN_DESKTOP;
  drawDesktop();
}




void drawMenu() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  tft.setTextSize(1); tft.setTextColor(COLOR_TEXT);
  tft.setCursor(4, 5); tft.print("PPC OS 2.8.5");
  tft.setCursor(139, 5); tft.print(WiFi.status() == WL_CONNECTED ? "W+" : "W-");

  if (selectedMenuItem < menuTop) menuTop = selectedMenuItem;
  if (selectedMenuItem >= menuTop + MENU_VISIBLE) menuTop = selectedMenuItem - MENU_VISIBLE + 1;

  for (int row = 0; row < MENU_VISIBLE; row++) {
    int i = menuTop + row; if (i >= MENU_ITEMS) break;
    int y = 23 + row * 17;
    uint16_t c = COLOR_GRAY;
    if (i == selectedMenuItem) {
      tft.fillRoundRect(4, y - 3, 152, 16, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    tft.setTextColor(c); tft.setCursor(8, y); tft.print(i + 1); tft.print(".");
    uiText(21, y + 7, menuItemText(i), c);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(7, 123, T("2/8 ВЫБОР   A ОТКРЫТЬ", "2/8 MOVE    A OPEN"), COLOR_GRAY);
}

void menuKey(char key) {
  if (key == '2') {
    selectedMenuItem--;
    if (selectedMenuItem < 0) selectedMenuItem = MENU_ITEMS - 1;
    drawMenu();
    return;
  }
  if (key == '8') {
    selectedMenuItem++;
    if (selectedMenuItem >= MENU_ITEMS) selectedMenuItem = 0;
    drawMenu();
    return;
  }
  if (key == 'A' || key == '#') { openSelectedMenuItem(); return; }
  if (key == 'D' || key == 'B') { goHome(); return; }
}

void openSelectedMenuItem() {
  if (selectedMenuItem == 0) {
    currentScreen = SCREEN_WIFI_MENU;
    drawWiFiMenu();
  } else if (selectedMenuItem == 1) {
    currentScreen = SCREEN_DATETIME_MENU;
    drawDateTimeMenu();
  } else if (selectedMenuItem == 2) {
    currentScreen = SCREEN_CALCULATIONS_MENU;
    calcMenuSelected = 0;
    drawCalculationsMenu();
  } else if (selectedMenuItem == 3) {
    currentScreen = SCREEN_RECORDS_MENU;
    recordsMenuSelected = 0;
    drawRecordsMenu();
  } else if (selectedMenuItem == 4) {
    currentScreen = SCREEN_FILES_MENU;
    drawFilesMenu();
  } else if (selectedMenuItem == 5) {
    currentScreen = SCREEN_GAMES_MENU;
    gamesMenuSelected = 0;
    drawGamesMenu();
  } else if (selectedMenuItem == 6) {
    currentScreen = SCREEN_PROGRAM_MENU;
    programMenuSelected = 0;
    ensureProgramFile();
    drawProgramMenu();
  } else if (selectedMenuItem == 7) {
    currentScreen = SCREEN_SETTINGS;
    settingsSelected = 0;
    drawSettings();
  }
}




void drawSettings() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("НАСТРОЙКИ", "SETTINGS"), COLOR_TEXT);



  const int count = 5;
  for (int i = 0; i < count; i++) {
    int y = 22 + i * 17;
    uint16_t c = COLOR_GRAY;
    if (i == settingsSelected) {
      tft.fillRoundRect(3, y - 2, 154, 16, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }

    if (i == 0) {
      uiText(8, y + 10, T("ЯЗЫК", "LANGUAGE"), c);
      String current = systemLanguage == LANG_RU ? "РУС" : "ENG";
      uiTextSmall(129, y + 9, current, c);
    } else if (i == 1) {
      uiText(8, y + 10, T("ТЕМА", "THEME"), c);
      String n = themeName(currentTheme);
      drawUtf8(100, y + 9, utf8Truncate(n, 9), c, u8g2_font_5x7_t_cyrillic);
    } else if (i == 2) {
      uiText(8, y + 10, T("РАБОЧИЙ СТОЛ", "DESKTOP"), c);
    } else if (i == 3) {
      uiText(8, y + 10, T("БЛОКИРОВКА", "LOCK"), c);
      uiTextSmall(129, y + 9, lockEnabled ? T("ВКЛ", "ON") : T("ВЫКЛ", "OFF"), c);
    } else {
      uiText(8, y + 10, T("О СИСТЕМЕ", "SYSTEM INFO"), c);
    }
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(4, 123, T("2/8 ВЫБОР A ОТКР D НАЗ", "2/8 MOVE A OPEN D BACK"), COLOR_GRAY);
}

void settingsKey(char key) {
  const int count = 5;
  if (key == '2') {
    settingsSelected--;
    if (settingsSelected < 0) settingsSelected = count - 1;
    drawSettings();
    return;
  }
  if (key == '8') {
    settingsSelected++;
    if (settingsSelected >= count) settingsSelected = 0;
    drawSettings();
    return;
  }
  if (key == 'A' || key == '#') {
    if (settingsSelected == 0) {
      languageSelected = systemLanguage == LANG_RU ? 0 : 1;
      currentScreen = SCREEN_LANGUAGE;
      drawLanguageSettings();
    } else if (settingsSelected == 1) {
      themeSelected = currentTheme;
      currentScreen = SCREEN_THEME;
      drawThemeSettings();
    } else if (settingsSelected == 2) {
      desktopSettingsSelected = 0;
      currentScreen = SCREEN_DESKTOP_SETTINGS;
      drawDesktopSettings();
    } else if (settingsSelected == 3) {
      lockSettingsSelected = 0;
      currentScreen = SCREEN_LOCK_SETTINGS;
      drawLockSettings();
    } else {
      systemPage = 0;
      currentScreen = SCREEN_SYSTEM;
      drawSystemInfo();
    }
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_MENU;
    drawMenu();
  }
}

void drawLanguageSettings() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ЯЗЫК", "LANGUAGE"), COLOR_TEXT);

  const char* langs[2] = { "РУССКИЙ", "ENGLISH" };
  for (int i = 0; i < 2; i++) {
    int y = 34 + i * 30;
    uint16_t c = COLOR_GRAY;
    if (i == languageSelected) {
      tft.fillRoundRect(5, y - 6, 150, 24, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    tft.setTextSize(1); tft.setTextColor(c); tft.setCursor(10, y); tft.print(i + 1); tft.print(".");
    uiText(27, y + 7, langs[i], c);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(5, 123, T("2/8 ВЫБОР A ПРИМ D НАЗ", "2/8 MOVE A APPLY D BACK"), COLOR_GRAY);
}

void languageKey(char key) {
  if (key == '2' || key == '8') {
    languageSelected = 1 - languageSelected;
    drawLanguageSettings();
    return;
  }
  if (key == 'A' || key == '#') {
    systemLanguage = languageSelected == 0 ? LANG_RU : LANG_EN;
    preferences.putUChar("lang", systemLanguage == LANG_RU ? 0 : 1);
    drawLanguageSettings();
    delay(220);
    currentScreen = SCREEN_SETTINGS;
    drawSettings();
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_SETTINGS;
    drawSettings();
  }
}




void applyTheme(uint8_t theme) {
  if (theme >= THEME_COUNT) theme = THEME_CLASSIC;
  currentTheme = theme;

  switch (theme) {
    case THEME_AMBER:

      themeBg     = 0x0820;
      themeText   = 0xFE8D;
      themeAccent = 0xFD40;
      themeSelect = 0x59A0;
      themeGreen  = 0xB6EA;
      themeRed    = 0xFA86;
      themeYellow = 0xFE80;
      themeGray   = 0x9366;
      break;

    case THEME_CLASSIC:
    default:

      themeBg     = ST77XX_BLACK;
      themeText   = ST77XX_WHITE;
      themeAccent = ST77XX_CYAN;
      themeSelect = ST77XX_BLUE;
      themeGreen  = ST77XX_GREEN;
      themeRed    = ST77XX_RED;
      themeYellow = ST77XX_YELLOW;
      themeGray   = 0x8410;
      break;
  }
}

String themeName(uint8_t theme) {
  switch (theme) {
    case THEME_AMBER:
      return T("ЯНТАРНАЯ", "AMBER");
    case THEME_CLASSIC:
    default:
      return T("КЛАССИЧЕСКАЯ", "CLASSIC");
  }
}

void drawThemeSettings() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ТЕМА", "THEME"), COLOR_TEXT);

  for (int i = 0; i < THEME_COUNT; i++) {
    int y = 34 + i * 28;
    uint16_t c = COLOR_GRAY;

    if (i == themeSelected) {
      tft.fillRoundRect(4, y - 7, 152, 22, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }

    tft.setTextSize(1);
    tft.setTextColor(c, COLOR_BG);
    tft.setCursor(8, y - 1);
    tft.print(i + 1);
    tft.print('.');
    uiText(24, y + 7, themeName(i), c);


    uint16_t accentPreview;
    uint16_t selectPreview;

    if (i == THEME_AMBER) {
      accentPreview = 0xFD40;
      selectPreview = 0x59A0;
    } else {
      accentPreview = ST77XX_CYAN;
      selectPreview = ST77XX_BLUE;
    }

    tft.fillRect(132, y - 4, 9, 9, accentPreview);
    tft.fillRect(144, y - 4, 9, 9, selectPreview);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(3, 122, T("2/8 ВЫБОР A ПРИМ D НАЗ", "2/8 MOVE A APPLY D BACK"), COLOR_GRAY);
}

void themeKey(char key) {
  if (key == '2') {
    themeSelected--;
    if (themeSelected < 0) themeSelected = THEME_COUNT - 1;
    drawThemeSettings();
    return;
  }

  if (key == '8') {
    themeSelected++;
    if (themeSelected >= THEME_COUNT) themeSelected = 0;
    drawThemeSettings();
    return;
  }

  if (key == 'A' || key == '#') {
    applyTheme(themeSelected);
    preferences.putUChar("theme", currentTheme);
    drawThemeSettings();
    return;
  }

  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_SETTINGS;
    drawSettings();
  }
}


void drawLockSettings() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("БЛОКИРОВКА", "LOCK"), COLOR_TEXT);

  const int count = 3;
  for (int i = 0; i < count; i++) {
    int y = 31 + i * 24;
    uint16_t c = COLOR_GRAY;
    if (i == lockSettingsSelected) {
      tft.fillRoundRect(4, y - 5, 152, 20, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }

    if (i == 0) {
      uiText(10, y + 8, T("ЗАЩИТА", "LOCK"), c);
      uiTextSmall(116, y + 8, lockEnabled ? T("ВКЛ", "ON") : T("ВЫКЛ", "OFF"), c);
    } else if (i == 1) {
      uiText(10, y + 8, T("ВРЕМЯ", "TIMEOUT"), c);
      String tm = String(lockTimeoutMinutes) + T(" МИН", " MIN");
      uiTextSmall(105, y + 8, tm, c);
    } else {
      uiText(10, y + 8, "PIN", c);
      String mask = "";
      for (int k = 0; k < systemPin.length(); k++) mask += '*';
      uiTextSmall(116, y + 8, mask, c);
    }
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(3, 122, T("A ИЗМЕН  2/8 ВЫБ  D НАЗ", "A CHANGE 2/8 MOVE D BACK"), COLOR_GRAY);
}

void lockSettingsKey(char key) {
  const int count = 3;

  if (key == '2') {
    lockSettingsSelected--;
    if (lockSettingsSelected < 0) lockSettingsSelected = count - 1;
    drawLockSettings();
    return;
  }

  if (key == '8') {
    lockSettingsSelected++;
    if (lockSettingsSelected >= count) lockSettingsSelected = 0;
    drawLockSettings();
    return;
  }

  if (key == 'A' || key == '#') {
    if (lockSettingsSelected == 0) {
      lockEnabled = !lockEnabled;
      if (!lockEnabled) systemLocked = false;
      markUserActivity();
      preferences.putBool("lockEn", lockEnabled);
      drawLockSettings();
      return;
    }

    if (lockSettingsSelected == 1) {
      uint8_t idx = currentLockTimeoutIndex();
      idx = (idx + 1) % LOCK_TIMEOUT_COUNT;
      lockTimeoutMinutes = lockTimeoutOptions[idx];
      preferences.putUInt("lockMin", lockTimeoutMinutes);
      markUserActivity();
      drawLockSettings();
      return;
    }

    lockPinEdit = "";
    currentScreen = SCREEN_LOCK_PIN_CHANGE;
    drawLockPinChange();
    return;
  }

  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_SETTINGS;
    drawSettings();
  }
}

void drawLockPinChange() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("НОВЫЙ PIN", "NEW PIN"), COLOR_TEXT);

  drawUiCentered(T("4-6 ЦИФР", "4-6 DIGITS"), 43, COLOR_GRAY, u8g2_font_6x12_t_cyrillic, 6);

  String mask = "";
  for (int i = 0; i < lockPinEdit.length(); i++) mask += '*';
  if (mask.length() == 0) mask = "_";
  drawUiCentered(mask, 71, COLOR_YELLOW, u8g2_font_6x12_t_cyrillic, 6);

  uiTextSmall(6, 96, T("0-9 ВВОД   B УДАЛИТЬ", "0-9 INPUT  B DELETE"), COLOR_GRAY);
  uiTextSmall(6, 108, T("C/A СОХРАНИТЬ", "C/A SAVE"), COLOR_GREEN);
  uiTextSmall(6, 121, T("D ОТМЕНА", "D CANCEL"), COLOR_GRAY);
}

void lockPinChangeKey(char key) {
  if (key >= '0' && key <= '9') {
    if (lockPinEdit.length() < 6) lockPinEdit += key;
    drawLockPinChange();
    return;
  }

  if (key == 'B') {
    if (lockPinEdit.length()) lockPinEdit.remove(lockPinEdit.length() - 1);
    drawLockPinChange();
    return;
  }

  if (key == 'C' || key == 'A' || key == '#') {
    if (lockPinEdit.length() < 4) {
      showMessage(T("PIN СЛИШКОМ КОРОТКИЙ", "PIN TOO SHORT"), T("НУЖНО 4-6 ЦИФР", "USE 4-6 DIGITS"), COLOR_RED, 750);
      drawLockPinChange();
      return;
    }

    systemPin = lockPinEdit;
    preferences.putString("sysPin", systemPin);
    lockPinEdit = "";
    showMessage(T("PIN СОХРАНЁН", "PIN SAVED"), "", COLOR_GREEN, 500);
    currentScreen = SCREEN_LOCK_SETTINGS;
    drawLockSettings();
    return;
  }

  if (key == 'D') {
    lockPinEdit = "";
    currentScreen = SCREEN_LOCK_SETTINGS;
    drawLockSettings();
  }
}


void drawDesktopSettings() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("РАБОЧИЙ СТОЛ", "DESKTOP"), COLOR_TEXT);

  const int count = 5;
  for (int i = 0; i < count; i++) {
    int y = 22 + i * 17;
    uint16_t c = COLOR_GRAY;
    if (i == desktopSettingsSelected) {
      tft.fillRoundRect(3, y - 2, 154, 16, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }

    if (i == 0) {
      uiText(8, y + 10, T("ОБОИ", "WALLPAPER"), c);
      String n = wallpaperPath.length() ? wallpaperPath : T("НЕТ", "NONE");
      if (n.startsWith("/")) n = n.substring(1);
      drawUtf8(64, y + 9, utf8Truncate(n, 15), c, u8g2_font_5x7_t_cyrillic);
    } else if (i == 1) {
      uiText(8, y + 10, T("ЧАСЫ", "CLOCK"), c);
      uiTextSmall(116, y + 9, desktopShowClock ? T("ВКЛ", "ON") : T("ВЫКЛ", "OFF"), c);
    } else if (i == 2) {
      uiText(8, y + 10, T("ДАТА", "DATE"), c);
      uiTextSmall(116, y + 9, desktopShowDate ? T("ВКЛ", "ON") : T("ВЫКЛ", "OFF"), c);
    } else if (i == 3) {
      uiText(8, y + 10, T("НАПОМИНАНИЯ", "REMINDERS"), c);
      uiTextSmall(126, y + 9, desktopShowReminders ? T("ВКЛ", "ON") : T("ВЫКЛ", "OFF"), c);
    } else {
      uiText(8, y + 10, T("ПОДСКАЗКА", "HINT"), c);
      uiTextSmall(116, y + 9, desktopShowHint ? T("ВКЛ", "ON") : T("ВЫКЛ", "OFF"), c);
    }
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(3, 123, T("2/8 ВЫБОР A ИЗМЕН D НАЗ", "2/8 MOVE A CHANGE D BACK"), COLOR_GRAY);
}

void desktopSettingsKey(char key) {
  const int count = 5;
  if (key == '2') {
    desktopSettingsSelected--;
    if (desktopSettingsSelected < 0) desktopSettingsSelected = count - 1;
    drawDesktopSettings();
    return;
  }
  if (key == '8') {
    desktopSettingsSelected++;
    if (desktopSettingsSelected >= count) desktopSettingsSelected = 0;
    drawDesktopSettings();
    return;
  }
  if (key == 'A' || key == '#') {
    if (desktopSettingsSelected == 0) {
      refreshWallpaperList();
      currentScreen = SCREEN_WALLPAPER_PICKER;
      drawWallpaperPicker();
      return;
    }
    if (desktopSettingsSelected == 1) {
      desktopShowClock = !desktopShowClock;
      preferences.putBool("deskClock", desktopShowClock);
    } else if (desktopSettingsSelected == 2) {
      desktopShowDate = !desktopShowDate;
      preferences.putBool("deskDate", desktopShowDate);
    } else if (desktopSettingsSelected == 3) {
      desktopShowReminders = !desktopShowReminders;
      preferences.putBool("deskEvents", desktopShowReminders);
    } else if (desktopSettingsSelected == 4) {
      desktopShowHint = !desktopShowHint;
      preferences.putBool("deskHint", desktopShowHint);
    }
    drawDesktopSettings();
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_SETTINGS;
    drawSettings();
  }
}

void refreshWallpaperList() {
  refreshFileList();
  wallpaperCount = 1;
  wallpaperFiles[0] = "";

  for (int i = 0; i < fileCount && wallpaperCount < MAX_WALLPAPERS; i++) {
    if (!isImageFile(fileNames[i])) continue;
    wallpaperFiles[wallpaperCount++] = fileNames[i];
  }

  wallpaperSelected = 0;
  for (int i = 1; i < wallpaperCount; i++) {
    if (wallpaperFiles[i] == wallpaperPath) {
      wallpaperSelected = i;
      break;
    }
  }
  wallpaperTop = max(0, wallpaperSelected - 1);
}

void drawWallpaperPicker() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ОБОИ", "WALLPAPER"), COLOR_TEXT);

  if (wallpaperSelected < wallpaperTop) wallpaperTop = wallpaperSelected;
  if (wallpaperSelected >= wallpaperTop + WALLPAPER_VISIBLE_ROWS)
    wallpaperTop = wallpaperSelected - WALLPAPER_VISIBLE_ROWS + 1;

  for (int row = 0; row < WALLPAPER_VISIBLE_ROWS; row++) {
    int i = wallpaperTop + row;
    if (i >= wallpaperCount) break;
    int y = 25 + row * 17;
    uint16_t c = COLOR_GRAY;
    if (i == wallpaperSelected) {
      tft.fillRoundRect(3, y - 3, 154, 16, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }

    String label;
    if (i == 0) label = T("НЕТ ОБОЕВ", "NO WALLPAPER");
    else {
      label = wallpaperFiles[i];
      if (label.startsWith("/")) label = label.substring(1);
    }
    drawUtf8(7, y + 9, utf8Truncate(label, 23), c, u8g2_font_5x7_t_cyrillic);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(4, 123, T("2/8 ВЫБОР A ПРИМ D НАЗ", "2/8 MOVE A APPLY D BACK"), COLOR_GRAY);
}

void applyWallpaper(const String& path) {
  wallpaperPath = path;
  preferences.putString("wallpaper", wallpaperPath);
}

void wallpaperPickerKey(char key) {
  if (key == '2') {
    wallpaperSelected--;
    if (wallpaperSelected < 0) wallpaperSelected = wallpaperCount - 1;
    drawWallpaperPicker();
    return;
  }
  if (key == '8') {
    wallpaperSelected++;
    if (wallpaperSelected >= wallpaperCount) wallpaperSelected = 0;
    drawWallpaperPicker();
    return;
  }
  if (key == 'A' || key == '#') {
    applyWallpaper(wallpaperFiles[wallpaperSelected]);
    currentScreen = SCREEN_DESKTOP_SETTINGS;
    drawDesktopSettings();
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_DESKTOP_SETTINGS;
    drawDesktopSettings();
  }
}





String passwordPresetName(int preset) {
  switch (preset) {
    case PASS_ALNUM:  return T("БУКВЫ + ЦИФРЫ", "ALNUM");
    case PASS_STRONG: return T("УСИЛЕННЫЙ", "STRONG");
    case PASS_DIGITS: return T("ТОЛЬКО ЦИФРЫ", "DIGITS");
    case PASS_HEX:    return "HEX";
    default:          return "";
  }
}

uint32_t ppcRandom32() {
  return esp_random();
}

char ppcRandomChar(const char* pool) {
  size_t n = strlen(pool);
  if (!n) return '?';
  return pool[ppcRandom32() % n];
}

void generatePassword() {
  const char* lower = "abcdefghijkmnopqrstuvwxyz";
  const char* upper = "ABCDEFGHJKLMNPQRSTUVWXYZ";
  const char* digits = "23456789";
  const char* symbols = "!@#$%&*+-_=?:";
  const char* hexPool = "0123456789ABCDEF";

  uint8_t length = passwordLengthOptions[passwordLengthIndex];
  generatedPassword = "";
  generatedPassword.reserve(length);

  String pool = "";
  if (passwordPreset == PASS_DIGITS) {
    pool = digits;
  } else if (passwordPreset == PASS_HEX) {
    pool = hexPool;
  } else {
    pool = String(lower) + upper + digits;
    if (passwordPreset == PASS_STRONG) pool += symbols;
    generatedPassword += ppcRandomChar(lower);
    generatedPassword += ppcRandomChar(upper);
    generatedPassword += ppcRandomChar(digits);
    if (passwordPreset == PASS_STRONG) generatedPassword += ppcRandomChar(symbols);
  }

  while (generatedPassword.length() < length) generatedPassword += ppcRandomChar(pool.c_str());

  for (int i = generatedPassword.length() - 1; i > 0; i--) {
    int j = ppcRandom32() % (i + 1);
    char a = generatedPassword[i];
    generatedPassword.setCharAt(i, generatedPassword[j]);
    generatedPassword.setCharAt(j, a);
  }
}

void drawPasswordGenerator() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ГЕНЕРАТОР ПАРОЛЕЙ", "PASSWORD GENERATOR"), COLOR_TEXT);

  tft.drawRoundRect(4, 22, 152, 39, 3, COLOR_GRAY);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_GREEN, COLOR_BG);
  String p1 = generatedPassword.substring(0, min(20, (int)generatedPassword.length()));
  String p2 = generatedPassword.length() > 20 ? generatedPassword.substring(20) : "";
  tft.setCursor(8, 30); tft.print(p1);
  if (p2.length()) { tft.setCursor(8, 44); tft.print(p2); }

  uiTextSmall(6, 76, T("РЕЖИМ:", "MODE:"), COLOR_GRAY);
  uiTextSmall(48, 76, passwordPresetName(passwordPreset), COLOR_YELLOW);
  uiTextSmall(6, 90, T("ДЛИНА:", "LENGTH:"), COLOR_GRAY);
  tft.setTextSize(1); tft.setTextColor(COLOR_YELLOW, COLOR_BG);
  tft.setCursor(48, 83); tft.print(passwordLengthOptions[passwordLengthIndex]);

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(2, 117, T("2/8 РЕЖ 4/6 ДЛИНА", "2/8 MODE 4/6 LENGTH"), COLOR_GRAY);
  uiTextSmall(2, 125, T("A/# НОВЫЙ D НАЗАД", "A/# NEW D BACK"), COLOR_GRAY);
}

void passwordGeneratorKey(char key) {
  if (key == '2') {
    passwordPreset--;
    if (passwordPreset < 0) passwordPreset = PASS_PRESET_COUNT - 1;
    generatePassword();
  } else if (key == '8') {
    passwordPreset++;
    if (passwordPreset >= PASS_PRESET_COUNT) passwordPreset = 0;
    generatePassword();
  } else if (key == '4') {
    passwordLengthIndex--;
    if (passwordLengthIndex < 0) passwordLengthIndex = PASSWORD_LENGTH_COUNT - 1;
    generatePassword();
  } else if (key == '6') {
    passwordLengthIndex++;
    if (passwordLengthIndex >= PASSWORD_LENGTH_COUNT) passwordLengthIndex = 0;
    generatePassword();
  } else if (key == 'A' || key == '#' || key == 'C') {
    generatePassword();
  } else if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_CALCULATIONS_MENU;
    drawCalculationsMenu();
    return;
  } else {
    return;
  }
  drawPasswordGenerator();
}




uint32_t crc32Value(const uint8_t* data, size_t len) {
  uint32_t crc = 0xFFFFFFFFUL;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; b++) {
      crc = (crc >> 1) ^ (0xEDB88320UL & (0UL - (crc & 1UL)));
    }
  }
  return ~crc;
}

String crc32Hex(const uint8_t* data, size_t len) {
  char buf[9];
  snprintf(buf, sizeof(buf), "%08lX", (unsigned long)crc32Value(data, len));
  return String(buf);
}


struct PpcSha1Ctx {
  uint8_t data[64];
  uint32_t datalen;
  uint64_t bitlen;
  uint32_t state[5];
};



uint32_t ppcSha1Rol(uint32_t value, uint8_t bits);
void ppcSha1Transform(PpcSha1Ctx& ctx, const uint8_t data[]);
void ppcSha1Init(PpcSha1Ctx& ctx);
void ppcSha1Update(PpcSha1Ctx& ctx, const uint8_t* data, size_t len);
void ppcSha1Final(PpcSha1Ctx& ctx, uint8_t hash[20]);

uint32_t ppcSha1Rol(uint32_t value, uint8_t bits) {
  return (value << bits) | (value >> (32 - bits));
}

void ppcSha1Transform(PpcSha1Ctx& ctx, const uint8_t data[]) {
  uint32_t w[80];
  for (uint8_t i = 0; i < 16; i++) {
    int j = i * 4;
    w[i] = ((uint32_t)data[j] << 24) |
           ((uint32_t)data[j + 1] << 16) |
           ((uint32_t)data[j + 2] << 8) |
           (uint32_t)data[j + 3];
  }
  for (uint8_t i = 16; i < 80; i++) {
    w[i] = ppcSha1Rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
  }

  uint32_t a = ctx.state[0];
  uint32_t b = ctx.state[1];
  uint32_t c = ctx.state[2];
  uint32_t d = ctx.state[3];
  uint32_t e = ctx.state[4];

  for (uint8_t i = 0; i < 80; i++) {
    uint32_t f, k;
    if (i < 20) {
      f = (b & c) | ((~b) & d);
      k = 0x5A827999UL;
    } else if (i < 40) {
      f = b ^ c ^ d;
      k = 0x6ED9EBA1UL;
    } else if (i < 60) {
      f = (b & c) | (b & d) | (c & d);
      k = 0x8F1BBCDCUL;
    } else {
      f = b ^ c ^ d;
      k = 0xCA62C1D6UL;
    }

    uint32_t temp = ppcSha1Rol(a, 5) + f + e + k + w[i];
    e = d;
    d = c;
    c = ppcSha1Rol(b, 30);
    b = a;
    a = temp;
  }

  ctx.state[0] += a;
  ctx.state[1] += b;
  ctx.state[2] += c;
  ctx.state[3] += d;
  ctx.state[4] += e;
}

void ppcSha1Init(PpcSha1Ctx& ctx) {
  ctx.datalen = 0;
  ctx.bitlen = 0;
  ctx.state[0] = 0x67452301UL;
  ctx.state[1] = 0xEFCDAB89UL;
  ctx.state[2] = 0x98BADCFEUL;
  ctx.state[3] = 0x10325476UL;
  ctx.state[4] = 0xC3D2E1F0UL;
}

void ppcSha1Update(PpcSha1Ctx& ctx, const uint8_t* data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    ctx.data[ctx.datalen++] = data[i];
    if (ctx.datalen == 64) {
      ppcSha1Transform(ctx, ctx.data);
      ctx.bitlen += 512ULL;
      ctx.datalen = 0;
    }
  }
}

void ppcSha1Final(PpcSha1Ctx& ctx, uint8_t hash[20]) {
  uint32_t i = ctx.datalen;

  if (i < 56) {
    ctx.data[i++] = 0x80;
    while (i < 56) ctx.data[i++] = 0x00;
  } else {
    ctx.data[i++] = 0x80;
    while (i < 64) ctx.data[i++] = 0x00;
    ppcSha1Transform(ctx, ctx.data);
    memset(ctx.data, 0, 56);
  }

  ctx.bitlen += (uint64_t)ctx.datalen * 8ULL;
  for (uint8_t b = 0; b < 8; b++) {
    ctx.data[63 - b] = (uint8_t)(ctx.bitlen >> (8 * b));
  }
  ppcSha1Transform(ctx, ctx.data);

  for (uint8_t word = 0; word < 5; word++) {
    for (uint8_t b = 0; b < 4; b++) {
      hash[word * 4 + b] = (uint8_t)(ctx.state[word] >> (24 - 8 * b));
    }
  }
}

String sha1Hex(const uint8_t* data, size_t len) {
  PpcSha1Ctx ctx;
  uint8_t digest[20];
  ppcSha1Init(ctx);
  ppcSha1Update(ctx, data, len);
  ppcSha1Final(ctx, digest);

  const char* hex = "0123456789ABCDEF";
  String out = "";
  out.reserve(40);
  for (uint8_t i = 0; i < 20; i++) {
    out += hex[digest[i] >> 4];
    out += hex[digest[i] & 0x0F];
  }
  return out;
}

struct PpcSha256Ctx {
  uint8_t data[64];
  uint32_t datalen;
  uint64_t bitlen;
  uint32_t state[8];
};




uint32_t ppcShaRotr(uint32_t x, uint8_t n);
void ppcShaTransform(PpcSha256Ctx& ctx, const uint8_t data[]);
void ppcShaInit(PpcSha256Ctx& ctx);
void ppcShaUpdate(PpcSha256Ctx& ctx, const uint8_t* data, size_t len);
void ppcShaFinal(PpcSha256Ctx& ctx, uint8_t hash[32]);

static const uint32_t ppcSha256K[64] = {
  0x428a2f98UL,0x71374491UL,0xb5c0fbcfUL,0xe9b5dba5UL,0x3956c25bUL,0x59f111f1UL,0x923f82a4UL,0xab1c5ed5UL,
  0xd807aa98UL,0x12835b01UL,0x243185beUL,0x550c7dc3UL,0x72be5d74UL,0x80deb1feUL,0x9bdc06a7UL,0xc19bf174UL,
  0xe49b69c1UL,0xefbe4786UL,0x0fc19dc6UL,0x240ca1ccUL,0x2de92c6fUL,0x4a7484aaUL,0x5cb0a9dcUL,0x76f988daUL,
  0x983e5152UL,0xa831c66dUL,0xb00327c8UL,0xbf597fc7UL,0xc6e00bf3UL,0xd5a79147UL,0x06ca6351UL,0x14292967UL,
  0x27b70a85UL,0x2e1b2138UL,0x4d2c6dfcUL,0x53380d13UL,0x650a7354UL,0x766a0abbUL,0x81c2c92eUL,0x92722c85UL,
  0xa2bfe8a1UL,0xa81a664bUL,0xc24b8b70UL,0xc76c51a3UL,0xd192e819UL,0xd6990624UL,0xf40e3585UL,0x106aa070UL,
  0x19a4c116UL,0x1e376c08UL,0x2748774cUL,0x34b0bcb5UL,0x391c0cb3UL,0x4ed8aa4aUL,0x5b9cca4fUL,0x682e6ff3UL,
  0x748f82eeUL,0x78a5636fUL,0x84c87814UL,0x8cc70208UL,0x90befffaUL,0xa4506cebUL,0xbef9a3f7UL,0xc67178f2UL
};

uint32_t ppcShaRotr(uint32_t x, uint8_t n) { return (x >> n) | (x << (32 - n)); }

void ppcShaTransform(PpcSha256Ctx& ctx, const uint8_t data[]) {
  uint32_t a,b,c,d,e,f,g,h,t1,t2,m[64];
  for (uint8_t i = 0, j = 0; i < 16; i++, j += 4) {
    m[i] = ((uint32_t)data[j] << 24) | ((uint32_t)data[j+1] << 16) |
           ((uint32_t)data[j+2] << 8) | (uint32_t)data[j+3];
  }
  for (uint8_t i = 16; i < 64; i++) {
    uint32_t s0 = ppcShaRotr(m[i-15],7) ^ ppcShaRotr(m[i-15],18) ^ (m[i-15] >> 3);
    uint32_t s1 = ppcShaRotr(m[i-2],17) ^ ppcShaRotr(m[i-2],19) ^ (m[i-2] >> 10);
    m[i] = m[i-16] + s0 + m[i-7] + s1;
  }
  a=ctx.state[0]; b=ctx.state[1]; c=ctx.state[2]; d=ctx.state[3];
  e=ctx.state[4]; f=ctx.state[5]; g=ctx.state[6]; h=ctx.state[7];

  for (uint8_t i = 0; i < 64; i++) {
    uint32_t S1 = ppcShaRotr(e,6) ^ ppcShaRotr(e,11) ^ ppcShaRotr(e,25);
    uint32_t ch = (e & f) ^ ((~e) & g);
    t1 = h + S1 + ch + ppcSha256K[i] + m[i];
    uint32_t S0 = ppcShaRotr(a,2) ^ ppcShaRotr(a,13) ^ ppcShaRotr(a,22);
    uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
    t2 = S0 + maj;
    h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
  }

  ctx.state[0]+=a; ctx.state[1]+=b; ctx.state[2]+=c; ctx.state[3]+=d;
  ctx.state[4]+=e; ctx.state[5]+=f; ctx.state[6]+=g; ctx.state[7]+=h;
}

void ppcShaInit(PpcSha256Ctx& ctx) {
  ctx.datalen = 0;
  ctx.bitlen = 0;
  ctx.state[0]=0x6a09e667UL; ctx.state[1]=0xbb67ae85UL;
  ctx.state[2]=0x3c6ef372UL; ctx.state[3]=0xa54ff53aUL;
  ctx.state[4]=0x510e527fUL; ctx.state[5]=0x9b05688cUL;
  ctx.state[6]=0x1f83d9abUL; ctx.state[7]=0x5be0cd19UL;
}

void ppcShaUpdate(PpcSha256Ctx& ctx, const uint8_t* data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    ctx.data[ctx.datalen++] = data[i];
    if (ctx.datalen == 64) {
      ppcShaTransform(ctx, ctx.data);
      ctx.bitlen += 512;
      ctx.datalen = 0;
    }
  }
}

void ppcShaFinal(PpcSha256Ctx& ctx, uint8_t hash[32]) {
  uint32_t i = ctx.datalen;
  if (ctx.datalen < 56) {
    ctx.data[i++] = 0x80;
    while (i < 56) ctx.data[i++] = 0x00;
  } else {
    ctx.data[i++] = 0x80;
    while (i < 64) ctx.data[i++] = 0x00;
    ppcShaTransform(ctx, ctx.data);
    memset(ctx.data, 0, 56);
  }

  ctx.bitlen += (uint64_t)ctx.datalen * 8ULL;
  ctx.data[63] = (uint8_t)(ctx.bitlen);
  ctx.data[62] = (uint8_t)(ctx.bitlen >> 8);
  ctx.data[61] = (uint8_t)(ctx.bitlen >> 16);
  ctx.data[60] = (uint8_t)(ctx.bitlen >> 24);
  ctx.data[59] = (uint8_t)(ctx.bitlen >> 32);
  ctx.data[58] = (uint8_t)(ctx.bitlen >> 40);
  ctx.data[57] = (uint8_t)(ctx.bitlen >> 48);
  ctx.data[56] = (uint8_t)(ctx.bitlen >> 56);
  ppcShaTransform(ctx, ctx.data);

  for (i = 0; i < 4; i++) {
    hash[i]      = (ctx.state[0] >> (24 - i * 8)) & 0xFF;
    hash[i + 4]  = (ctx.state[1] >> (24 - i * 8)) & 0xFF;
    hash[i + 8]  = (ctx.state[2] >> (24 - i * 8)) & 0xFF;
    hash[i + 12] = (ctx.state[3] >> (24 - i * 8)) & 0xFF;
    hash[i + 16] = (ctx.state[4] >> (24 - i * 8)) & 0xFF;
    hash[i + 20] = (ctx.state[5] >> (24 - i * 8)) & 0xFF;
    hash[i + 24] = (ctx.state[6] >> (24 - i * 8)) & 0xFF;
    hash[i + 28] = (ctx.state[7] >> (24 - i * 8)) & 0xFF;
  }
}

String sha256Hex(const uint8_t* data, size_t len) {
  PpcSha256Ctx ctx;
  uint8_t digest[32];
  ppcShaInit(ctx);
  ppcShaUpdate(ctx, data, len);
  ppcShaFinal(ctx, digest);
  const char* hex = "0123456789ABCDEF";
  String out = "";
  out.reserve(64);
  for (uint8_t i = 0; i < 32; i++) {
    out += hex[digest[i] >> 4];
    out += hex[digest[i] & 0x0F];
  }
  return out;
}

void drawHashHelp() {
  const int x0 = 104;
  tft.fillRect(102, 18, 58, 89, COLOR_BG);
  tft.drawFastVLine(101, 18, 89, COLOR_GRAY);
  int y = 25;
  auto line = [&](const String& s) {
    drawUtf8(x0, y, s, COLOR_YELLOW, u8g2_font_5x7_t_cyrillic);
    y += 8;
  };

  if (hashInputMode == INPUT_RUS_LOWER || hashInputMode == INPUT_RUS_UPPER) {
    bool up = hashInputMode == INPUT_RUS_UPPER;
    line("1 .,!?"); line(up ? "2 АБВГ" : "2 абвг"); line(up ? "3 ДЕЁЖЗ" : "3 деёжз");
    line(up ? "4 ИЙКЛ" : "4 ийкл"); line(up ? "5 МНОП" : "5 мноп"); line(up ? "6 РСТУ" : "6 рсту");
    line(up ? "7 ФХЦЧ" : "7 фхцч"); line(up ? "8 ШЩЪЫ" : "8 шщъы"); line(up ? "9 ЬЭЮЯ" : "9 ьэюя");
    line(T("0 пробел", "0 space")); line("* @._-"); line(T("# фикс", "# commit"));
  } else if (hashInputMode == INPUT_NAV) {
    line(T("НАВИГАЦИЯ", "NAVIGATION")); line(T("4 <  6 >", "4 <  6 >"));
    line(T("2 ^  8 v", "2 ^  8 v")); line(T("A режим", "A mode"));
    line(T("B удалить", "B delete")); line(T("C hash", "C hash"));
  } else if (hashInputMode == INPUT_NUMBERS) {
    line(T("РЕЖИМ 123", "MODE 123")); line("0..9"); line(T("A режим", "A mode"));
    line(T("B удалить", "B delete")); line(T("C hash", "C hash")); line(T("# фикс", "# commit"));
  } else {
    bool up = hashInputMode == INPUT_LAT_UPPER;
    line("1 .,!?"); line(up ? "2 ABC" : "2 abc"); line(up ? "3 DEF" : "3 def"); line(up ? "4 GHI" : "4 ghi");
    line(up ? "5 JKL" : "5 jkl"); line(up ? "6 MNO" : "6 mno"); line(up ? "7 PQRS" : "7 pqrs");
    line(up ? "8 TUV" : "8 tuv"); line(up ? "9 WXYZ" : "9 wxyz"); line(T("0 пробел", "0 space"));
    line("* @._-"); line(T("# фикс", "# commit"));
  }
}

void drawHashInput() {
  tft.fillRect(0, 18, 100, 89, COLOR_BG);
  const int cols = 16, rows = 6, x0 = 3, y0 = 30;
  if (hashCursor < 0) hashCursor = 0;
  if (hashCursor > hashInput.length()) hashCursor = hashInput.length();
  int cursorGlyph = sharedCursorGlyphIndex(hashInput, hashCursor);
  int firstGlyph = 0;
  if (cursorGlyph >= cols * rows) firstGlyph = (cursorGlyph / cols - rows + 1) * cols;
  int i = sharedCursorByteForGlyph(hashInput, firstGlyph);
  int glyphNo = firstGlyph, line = 0, col = 0;
  while (i < hashInput.length() && line < rows) {
    if (glyphNo == cursorGlyph) tft.drawFastVLine(x0 + col*6, 20 + line*12, 11, COLOR_ACCENT);
    String g = utf8GlyphAt(hashInput, i);
    drawUtf8(x0 + col * 6, y0 + line * 12, g, COLOR_TEXT, u8g2_font_6x12_t_cyrillic);
    i = utf8NextIndex(hashInput, i); glyphNo++; col++;
    if (col >= cols) { col = 0; line++; }
  }
  if (cursorGlyph >= glyphNo && line < rows) tft.drawFastVLine(x0 + col*6, 20 + line*12, 11, COLOR_ACCENT);
}

void drawHashCalculator() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("HASH-КАЛЬКУЛЯТОР", "HASH CALCULATOR"), COLOR_TEXT);

  if (hashShowingResult) {
    uiTextSmall(3, 27, "CRC32:", COLOR_GRAY);
    tft.setTextSize(1); tft.setTextColor(COLOR_GREEN, COLOR_BG);
    tft.setCursor(42, 20); tft.print(hashCrc32);

    uiTextSmall(3, 40, "SHA-1:", COLOR_GRAY);
    tft.setTextSize(1); tft.setTextColor(COLOR_TEXT, COLOR_BG);
    for (int row = 0; row < 2; row++) {
      String part = hashSha1.substring(row * 20, row * 20 + 20);
      tft.setCursor(8, 42 + row * 9);
      tft.print(part);
    }

    uiTextSmall(3, 67, "SHA-256:", COLOR_GRAY);
    tft.setTextSize(1); tft.setTextColor(COLOR_TEXT, COLOR_BG);
    for (int row = 0; row < 4; row++) {
      String part = hashSha256.substring(row * 16, row * 16 + 16);
      tft.setCursor(8, 69 + row * 9);
      tft.print(part);
    }

    tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
    uiTextSmall(3, 117, T("B/C НАЗАД", "B/C BACK"), COLOR_GRAY);
    uiTextSmall(104, 117, T("БАЙТ:", "BYTES:"), COLOR_GRAY);
    tft.setTextSize(1); tft.setTextColor(COLOR_YELLOW, COLOR_BG);
    tft.setCursor(134, 110); tft.print(hashInput.length());
    uiTextSmall(3, 125, T("D ВЫХОД", "D EXIT"), COLOR_GRAY);
    return;
  }

  drawHashInput();
  drawHashHelp();
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(2, 116, T("A РЕЖ B УДАЛ C HASH", "A MODE B DEL C HASH"), COLOR_GRAY);
  uiTextSmall(2, 125, T("D НАЗ # ФИКС", "D BACK # COMMIT"), COLOR_GRAY);
  uiTextSmall(121, 125, inputModeName(hashInputMode), COLOR_YELLOW);
}

void hashMultiTap(char key) {
  if (hashInputMode == INPUT_NAV) return;
  if (hashInputMode == INPUT_NUMBERS && key >= '0' && key <= '9') {
    sharedInsert(hashInput, hashCursor, String(key), MAX_HASH_INPUT_BYTES);
    hashMultiActive = false; drawHashCalculator(); return;
  }
  String set = getCharacterSet(key, hashInputMode);
  if (!set.length()) return;
  unsigned long now = millis();
  bool sameKey = hashMultiActive && key == hashLastKey && now - hashLastTime < 900;
  if (sameKey && hashCursor > 0) {
    int count = utf8Count(set); hashMultiIndex = (hashMultiIndex + 1) % max(1, count);
    utf8ReplacePrevious(hashInput, hashCursor, utf8GlyphAtNumber(set, hashMultiIndex));
  } else {
    hashMultiIndex = 0; sharedInsert(hashInput, hashCursor, utf8GlyphAtNumber(set, 0), MAX_HASH_INPUT_BYTES);
  }
  hashLastKey = key; hashLastTime = now; hashMultiActive = true; drawHashCalculator();
}

void hashCalculatorKey(char key) {
  if (hashShowingResult) {
    if (key == 'B' || key == 'C' || key == '#') {
      hashShowingResult = false;
      drawHashCalculator();
      return;
    }
    if (key == 'D') {
      currentScreen = SCREEN_CALCULATIONS_MENU;
      drawCalculationsMenu();
      return;
    }
    return;
  }

  if (key == 'A') {
    hashMultiActive = false;
    if (hashInputMode == INPUT_LAT_LOWER) hashInputMode = INPUT_LAT_UPPER;
    else if (hashInputMode == INPUT_LAT_UPPER) hashInputMode = INPUT_RUS_LOWER;
    else if (hashInputMode == INPUT_RUS_LOWER) hashInputMode = INPUT_RUS_UPPER;
    else if (hashInputMode == INPUT_RUS_UPPER) hashInputMode = INPUT_NUMBERS;
    else if (hashInputMode == INPUT_NUMBERS) hashInputMode = INPUT_NAV;
    else hashInputMode = INPUT_LAT_LOWER;
    drawHashCalculator();
    return;
  }

  if (hashInputMode == INPUT_NAV && (key == '2' || key == '8' || key == '4' || key == '6')) {
    hashMultiActive = false;
    if (key == '4') sharedCursorMoveHorizontal(hashInput, hashCursor, -1);
    else if (key == '6') sharedCursorMoveHorizontal(hashInput, hashCursor, 1);
    else if (key == '2') sharedCursorMoveVertical(hashInput, hashCursor, 16, -1);
    else if (key == '8') sharedCursorMoveVertical(hashInput, hashCursor, 16, 1);
    drawHashCalculator(); return;
  }

  if (key == 'B') {
    hashMultiActive = false;
    if (sharedBackspace(hashInput, hashCursor)) drawHashCalculator();
    return;
  }

  if (key == 'C') {
    hashMultiActive = false;
    const uint8_t* bytes = (const uint8_t*)hashInput.c_str();
    size_t len = hashInput.length();
    hashCrc32 = crc32Hex(bytes, len);
    hashSha1 = sha1Hex(bytes, len);
    hashSha256 = sha256Hex(bytes, len);
    hashShowingResult = true;
    drawHashCalculator();
    return;
  }

  if (key == '#') {
    hashMultiActive = false;
    return;
  }

  if (key == 'D') {
    currentScreen = SCREEN_CALCULATIONS_MENU;
    drawCalculationsMenu();
    return;
  }

  hashMultiTap(key);
}





void ensureProgramFile() {
  if (LittleFS.exists(PROGRAM_FILE)) return;

  File f = LittleFS.open(PROGRAM_FILE, "w");
  if (!f) return;

  f.println("REM PPC SCRIPT DEMO");
  f.println("SET A 1");
  f.println("PRINT START");
  f.println("PRINT A");
  f.println("ADD A 1");
  f.println("IF A <= 5 GOTO 4");
  f.println("PRINT DONE");
  f.println("END");
  f.close();
}

void drawProgramMenu() {
  ensureProgramFile();
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, "PROGRAM", COLOR_TEXT);

  const char* ru[PROGRAM_MENU_ITEMS] = { "РЕДАКТОР", "ЗАПУСТИТЬ", "СПРАВКА" };
  const char* en[PROGRAM_MENU_ITEMS] = { "EDITOR", "RUN", "HELP" };

  for (int i = 0; i < PROGRAM_MENU_ITEMS; i++) {
    int y = 33 + i * 24;
    uint16_t c = COLOR_GRAY;
    if (i == programMenuSelected) {
      tft.fillRoundRect(5, y - 5, 150, 20, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    tft.setTextSize(1);
    tft.setTextColor(c, COLOR_BG);
    tft.setCursor(10, y);
    tft.print(i + 1);
    tft.print(".");
    uiText(27, y + 7, systemLanguage == LANG_RU ? ru[i] : en[i], c);
  }

  File f = LittleFS.open(PROGRAM_FILE, "r");
  size_t bytes = f ? f.size() : 0;
  if (f) f.close();
  uiTextSmall(7, 105, T("ФАЙЛ: /PROGRAM.PPC", "FILE: /PROGRAM.PPC"), COLOR_GRAY);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_YELLOW, COLOR_BG);
  tft.setCursor(121, 98);
  tft.print(bytes);
  tft.print("B");

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(5, 123, T("2/8 ВЫБ A ОТКР D НАЗ", "2/8 MOVE A OPEN D BACK"), COLOR_GRAY);
}

void openProgramEditor() {
  ensureProgramFile();
  editorReturnScreen = SCREEN_PROGRAM_MENU;
  openNote(String(PROGRAM_FILE));
  editorMode = INPUT_LAT_UPPER;
  drawEditor();
}

void programMenuKey(char key) {
  if (key == '2') {
    programMenuSelected--;
    if (programMenuSelected < 0) programMenuSelected = PROGRAM_MENU_ITEMS - 1;
    drawProgramMenu();
    return;
  }

  if (key == '8') {
    programMenuSelected++;
    if (programMenuSelected >= PROGRAM_MENU_ITEMS) programMenuSelected = 0;
    drawProgramMenu();
    return;
  }

  if (key == 'A' || key == '#') {
    if (programMenuSelected == 0) {
      openProgramEditor();
    } else if (programMenuSelected == 1) {
      startProgram();
    } else {
      programHelpPage = 0;
      currentScreen = SCREEN_PROGRAM_HELP;
      drawProgramHelp();
    }
    return;
  }

  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_MENU;
    drawMenu();
  }
}

int programCountLines(const String& source) {
  if (!source.length()) return 0;
  int count = 1;
  for (int i = 0; i < source.length(); i++) if (source[i] == '\n') count++;
  return count;
}

String programGetLine(int index) {
  if (index < 0) return "";
  int current = 0;
  int start = 0;

  for (int i = 0; i <= programSource.length(); i++) {
    bool end = (i == programSource.length()) || programSource[i] == '\n';
    if (!end) continue;

    if (current == index) {
      String line = programSource.substring(start, i);
      if (line.endsWith("\r")) line.remove(line.length() - 1);
      return line;
    }

    current++;
    start = i + 1;
  }
  return "";
}

String programToken(const String& s, int tokenIndex) {
  int found = 0;
  int i = 0;
  while (i < s.length()) {
    while (i < s.length() && (s[i] == ' ' || s[i] == '\t')) i++;
    if (i >= s.length()) break;
    int start = i;
    while (i < s.length() && s[i] != ' ' && s[i] != '\t') i++;
    if (found == tokenIndex) return s.substring(start, i);
    found++;
  }
  return "";
}

String programRestAfterFirstToken(const String& s) {
  int i = 0;
  while (i < s.length() && (s[i] == ' ' || s[i] == '\t')) i++;
  while (i < s.length() && s[i] != ' ' && s[i] != '\t') i++;
  while (i < s.length() && (s[i] == ' ' || s[i] == '\t')) i++;
  if (i >= s.length()) return "";
  return s.substring(i);
}

bool programRegisterIndex(const String& token, int& index) {
  String t = token;
  t.trim();
  t.toUpperCase();
  if (t.length() != 1 || t[0] < 'A' || t[0] > 'Z') return false;
  index = t[0] - 'A';
  return true;
}

int32_t programResolveValue(const String& token, bool& ok) {
  String t = token;
  t.trim();

  int reg = -1;
  if (programRegisterIndex(t, reg)) {
    ok = true;
    return programRegs[reg];
  }

  if (!t.length()) {
    ok = false;
    return 0;
  }

  char* endPtr = nullptr;
  long v = strtol(t.c_str(), &endPtr, 0);
  ok = endPtr && *endPtr == '\0';
  return (int32_t)v;
}

void programPushOutput(const String& line) {
  String shown = line;
  if (utf8Count(shown) > 25) shown = utf8Truncate(shown, 25);

  if (programOutputCount < PROGRAM_OUTPUT_LINES) {


    int row = programOutputCount;
    programOutput[programOutputCount++] = shown;
    tft.fillRect(0, 20 + row * 10, 160, 10, COLOR_BG);
    drawUtf8(4, 29 + row * 10, shown, COLOR_TEXT, u8g2_font_5x7_t_cyrillic);
  } else {


    for (int i = 0; i < PROGRAM_OUTPUT_LINES - 1; i++) programOutput[i] = programOutput[i + 1];
    programOutput[PROGRAM_OUTPUT_LINES - 1] = shown;
    programDrawOutput();
  }
}

void programDrawOutput() {
  tft.fillRect(0, 18, 160, 77, COLOR_BG);
  for (int i = 0; i < programOutputCount; i++) {
    drawUtf8(4, 29 + i * 10, programOutput[i], COLOR_TEXT, u8g2_font_5x7_t_cyrillic);
  }
}

void programDrawStatus() {

  String state;
  uint16_t color = COLOR_GRAY;
  if (programRunning && programPaused) {
    state = T("ПАУЗА", "PAUSED");
    color = COLOR_YELLOW;
  } else if (programRunning) {
    state = T("ВЫПОЛНЕНИЕ", "RUNNING");
    color = COLOR_GREEN;
  } else if (programFinished) {
    state = programStatus.length() ? programStatus : "END";
    color = programStatus.startsWith("ERR") ? COLOR_RED : COLOR_GREEN;
  } else {
    state = T("ГОТОВО", "READY");
  }

  if (!programStatusUiInitialized) {


    tft.fillRect(0, 95, 160, 33, COLOR_BG);
    tft.drawFastHLine(0, 96, 160, COLOR_GRAY);
    programStatusUiInitialized = true;
    programLastStateDrawn = "";
    programLastLineDrawn = -1;
    programLastControlValid = false;
    programLastLineUiAt = 0;
  }


  if (state != programLastStateDrawn) {
    tft.fillRect(0, 98, 108, 13, COLOR_BG);
    drawUtf8(4, 107, state, color, u8g2_font_5x7_t_cyrillic);
    programLastStateDrawn = state;
  }

  int shownLine = programPc + 1;
  if (programLineCount > 0) {
    if (shownLine < 1) shownLine = 1;
    if (shownLine > programLineCount) shownLine = programLineCount;
  }



  unsigned long now = millis();
  bool lineRefreshDue = !programRunning || programLastLineDrawn < 0 ||
                        (now - programLastLineUiAt >= 120);
  if (shownLine != programLastLineDrawn && lineRefreshDue) {
    tft.fillRect(109, 98, 51, 13, COLOR_BG);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_YELLOW, COLOR_BG);
    tft.setCursor(111, 100);
    tft.print("L");
    tft.print(shownLine);
    programLastLineDrawn = shownLine;
    programLastLineUiAt = now;
  }


  bool controlsRunning = programRunning;
  if (!programLastControlValid || controlsRunning != programLastControlRunning) {
    tft.fillRect(0, 112, 160, 16, COLOR_BG);
    if (controlsRunning) {
      uiTextSmall(4, 124, T("A ПАУЗА  C CLS  D СТОП", "A PAUSE  C CLS  D STOP"), COLOR_GRAY);
    } else {
      uiTextSmall(4, 124, T("A СНОВА   D НАЗАД", "A AGAIN   D BACK"), COLOR_GRAY);
    }
    programLastControlRunning = controlsRunning;
    programLastControlValid = true;
  }
}

void drawProgramRun() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("PROGRAM: ВЫПОЛНЕНИЕ", "PROGRAM: RUN"), COLOR_TEXT);


  programStatusUiInitialized = false;
  programLastStateDrawn = "";
  programLastLineDrawn = -1;
  programLastControlValid = false;
  programLastLineUiAt = 0;

  programDrawOutput();
  programDrawStatus();
}

void programFail(int lineNumber, const String& message) {
  programRunning = false;
  programPaused = false;
  programFinished = true;
  programStatus = "ERR L" + String(lineNumber) + ": " + message;
  programPushOutput(programStatus);
  programDrawStatus();
}

void startProgram() {
  ensureProgramFile();
  File f = LittleFS.open(PROGRAM_FILE, "r");
  if (!f) {
    showMessage(T("ОШИБКА", "ERROR"), T("НЕТ PROGRAM.PPC", "NO PROGRAM.PPC"), COLOR_RED, 700);
    currentScreen = SCREEN_PROGRAM_MENU;
    drawProgramMenu();
    return;
  }

  programSource = f.readString();
  f.close();

  memset(programRegs, 0, sizeof(programRegs));
  for (int i = 0; i < PROGRAM_OUTPUT_LINES; i++) programOutput[i] = "";
  programOutputCount = 0;
  programPc = 0;
  programLineCount = programCountLines(programSource);
  programRunning = true;
  programPaused = false;
  programFinished = false;
  programStatus = "";
  programNextStepAt = 0;
  programExecutedSteps = 0;

  currentScreen = SCREEN_PROGRAM_RUN;
  drawProgramRun();

  if (programLineCount == 0) {
    programFail(1, T("ПУСТО", "EMPTY"));
  }
}

void executeProgramLine(const String& rawLine, int sourceLineNumber) {
  String line = rawLine;
  line.trim();
  if (!line.length()) return;

  String cmd = programToken(line, 0);
  cmd.toUpperCase();

  if (cmd == "REM" || cmd == "//") return;

  if (cmd == "END" || cmd == "STOP") {
    programRunning = false;
    programFinished = true;
    programStatus = "END";
    programPushOutput("END");
    return;
  }

  if (cmd == "CLS") {
    for (int i = 0; i < PROGRAM_OUTPUT_LINES; i++) programOutput[i] = "";
    programOutputCount = 0;
    programDrawOutput();
    return;
  }

  if (cmd == "PRINT") {
    String rest = programRestAfterFirstToken(line);
    rest.trim();
    if (rest.length() >= 2 && rest[0] == '"' && rest[rest.length() - 1] == '"') {
      rest = rest.substring(1, rest.length() - 1);
      programPushOutput(rest);
      return;
    }

    int reg = -1;
    if (programRegisterIndex(rest, reg)) {
      programPushOutput(String((long)programRegs[reg]));
    } else {
      programPushOutput(rest);
    }
    return;
  }

  if (cmd == "WAIT") {
    bool ok = false;
    int32_t ms = programResolveValue(programToken(line, 1), ok);
    if (!ok || ms < 0) {
      programFail(sourceLineNumber, "WAIT");
      return;
    }
    if (ms > 60000) ms = 60000;
    programNextStepAt = millis() + (unsigned long)ms;
    return;
  }

  if (cmd == "GOTO") {
    bool ok = false;
    int32_t target = programResolveValue(programToken(line, 1), ok);
    if (!ok || target < 1 || target > programLineCount) {
      programFail(sourceLineNumber, "GOTO");
      return;
    }
    programPc = (int)target - 1;
    return;
  }

  if (cmd == "IF") {
    String leftToken = programToken(line, 1);
    String op = programToken(line, 2);
    String rightToken = programToken(line, 3);
    String gotoToken = programToken(line, 4);
    String targetToken = programToken(line, 5);
    gotoToken.toUpperCase();

    bool okLeft = false, okRight = false, okTarget = false;
    int32_t left = programResolveValue(leftToken, okLeft);
    int32_t right = programResolveValue(rightToken, okRight);
    int32_t target = programResolveValue(targetToken, okTarget);

    if (!okLeft || !okRight || gotoToken != "GOTO" || !okTarget ||
        target < 1 || target > programLineCount) {
      programFail(sourceLineNumber, "IF");
      return;
    }

    bool condition = false;
    if (op == "==") condition = left == right;
    else if (op == "!=") condition = left != right;
    else if (op == ">") condition = left > right;
    else if (op == "<") condition = left < right;
    else if (op == ">=") condition = left >= right;
    else if (op == "<=") condition = left <= right;
    else {
      programFail(sourceLineNumber, "IF OP");
      return;
    }

    if (condition) programPc = (int)target - 1;
    return;
  }

  if (cmd == "INC" || cmd == "DEC") {
    int reg = -1;
    if (!programRegisterIndex(programToken(line, 1), reg)) {
      programFail(sourceLineNumber, cmd);
      return;
    }
    programRegs[reg] += (cmd == "INC") ? 1 : -1;
    return;
  }

  if (cmd == "SET" || cmd == "ADD" || cmd == "SUB" || cmd == "MUL" ||
      cmd == "DIV" || cmd == "MOD") {
    int reg = -1;
    if (!programRegisterIndex(programToken(line, 1), reg)) {
      programFail(sourceLineNumber, cmd);
      return;
    }

    bool ok = false;
    int32_t value = programResolveValue(programToken(line, 2), ok);
    if (!ok) {
      programFail(sourceLineNumber, cmd);
      return;
    }

    if (cmd == "SET") programRegs[reg] = value;
    else if (cmd == "ADD") programRegs[reg] += value;
    else if (cmd == "SUB") programRegs[reg] -= value;
    else if (cmd == "MUL") programRegs[reg] *= value;
    else if (cmd == "DIV") {
      if (value == 0) {
        programFail(sourceLineNumber, "DIV0");
        return;
      }
      programRegs[reg] /= value;
    } else if (cmd == "MOD") {
      if (value == 0) {
        programFail(sourceLineNumber, "MOD0");
        return;
      }
      programRegs[reg] %= value;
    }
    return;
  }

  programFail(sourceLineNumber, T("КОМАНДА", "COMMAND"));
}

void updateProgramRun() {
  if (!programRunning || programPaused) return;
  if ((long)(millis() - programNextStepAt) < 0) return;

  if (programPc < 0 || programPc >= programLineCount) {
    programRunning = false;
    programFinished = true;
    programStatus = "END";
    programDrawStatus();
    return;
  }

  int lineNumber = programPc + 1;
  String line = programGetLine(programPc);


  programPc++;
  programExecutedSteps++;
  executeProgramLine(line, lineNumber);

  if (programRunning) {
    if (programNextStepAt == 0 || (long)(millis() - programNextStepAt) >= 0) {
      programNextStepAt = millis() + 35;
    }
  }

  programDrawStatus();
}

void programRunKey(char key) {
  if (key == 'D' || key == 'B') {
    if (programRunning) {
      programRunning = false;
      programPaused = false;
      programFinished = true;
      programStatus = T("ОСТАНОВЛЕНО", "STOPPED");
    }
    currentScreen = SCREEN_PROGRAM_MENU;
    drawProgramMenu();
    return;
  }

  if (key == 'C') {
    for (int i = 0; i < PROGRAM_OUTPUT_LINES; i++) programOutput[i] = "";
    programOutputCount = 0;
    programDrawOutput();
    return;
  }

  if (key == 'A') {
    if (programRunning) {
      programPaused = !programPaused;
      programDrawStatus();
    } else {
      startProgram();
    }
  }
}

void drawProgramHelp() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("PROGRAM: СПРАВКА", "PROGRAM: HELP"), COLOR_TEXT);

  if (programHelpPage == 0) {
    uiTextSmall(4, 29, "SET A 10", COLOR_TEXT);
    uiTextSmall(4, 40, "ADD/SUB/MUL/DIV A 2", COLOR_TEXT);
    uiTextSmall(4, 51, "MOD A 3   INC/DEC A", COLOR_TEXT);
    uiTextSmall(4, 62, "PRINT A / PRINT HELLO", COLOR_TEXT);
    uiTextSmall(4, 73, "GOTO 4", COLOR_TEXT);
    uiTextSmall(4, 84, "IF A >= 5 GOTO 8", COLOR_TEXT);
    uiTextSmall(4, 95, "WAIT 500  CLS  END  REM", COLOR_TEXT);
  } else {
    uiTextSmall(4, 28, T("ПРИМЕР:", "EXAMPLE:"), COLOR_YELLOW);
    uiTextSmall(4, 39, "SET A 1", COLOR_TEXT);
    uiTextSmall(4, 50, "PRINT A", COLOR_TEXT);
    uiTextSmall(4, 61, "ADD A 1", COLOR_TEXT);
    uiTextSmall(4, 72, "IF A <= 5 GOTO 2", COLOR_TEXT);
    uiTextSmall(4, 83, "PRINT DONE", COLOR_TEXT);
    uiTextSmall(4, 94, "END", COLOR_TEXT);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  String page = String(programHelpPage + 1) + "/2";
  uiTextSmall(4, 123, T("2/8 СТРАНИЦА D НАЗАД", "2/8 PAGE D BACK"), COLOR_GRAY);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_YELLOW, COLOR_BG);
  tft.setCursor(140, 116);
  tft.print(page);
}

void programHelpKey(char key) {
  if (key == '2' || key == '8' || key == 'A' || key == '#') {
    programHelpPage = 1 - programHelpPage;
    drawProgramHelp();
    return;
  }

  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_PROGRAM_MENU;
    drawProgramMenu();
  }
}




void drawDateTimeMenu() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("ДАТА И ВРЕМЯ", "DATE & TIME"), COLOR_TEXT);
  for (int i = 0; i < DATETIME_ITEMS; i++) {
    int y = 28 + i * 20; uint16_t c = COLOR_GRAY;
    if (i == dateTimeSelected) {
      tft.fillRoundRect(5, y - 4, 150, 18, 3, COLOR_SELECT); c = COLOR_TEXT;
    }
    tft.setTextColor(c); tft.setCursor(10, y); tft.print(i + 1); tft.print(".");
    uiText(24, y + 7, dateTimeItemText(i), c);
  }
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(4, 123, T("2/8 ВЫБОР A ОТКР D НАЗАД", "2/8 MOVE A OPEN D BACK"), COLOR_GRAY);
}

void dateTimeMenuKey(char key) {
  if (key == '2') {
    dateTimeSelected--;
    if (dateTimeSelected < 0) dateTimeSelected = DATETIME_ITEMS - 1;
    drawDateTimeMenu(); return;
  }
  if (key == '8') {
    dateTimeSelected++;
    if (dateTimeSelected >= DATETIME_ITEMS) dateTimeSelected = 0;
    drawDateTimeMenu(); return;
  }
  if (key == 'A' || key == '#') {
    if (dateTimeSelected == 0) {
      currentScreen = SCREEN_CLOCK;
      oldTimeString = "        "; oldDateString = "          ";
      drawClockScreen();
    } else if (dateTimeSelected == 1) {
      currentScreen = SCREEN_TIMER;
      drawTimerScreen();
    } else if (dateTimeSelected == 2) {
      currentScreen = SCREEN_STOPWATCH;
      drawStopwatchScreen();
    } else {
      initCalendarToToday();
      currentScreen = SCREEN_CALENDAR;
      drawCalendar();
    }
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_MENU;
    drawMenu();
  }
}




void drawCalculationsMenu() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ВЫЧИСЛЕНИЯ", "CALCULATIONS"), COLOR_TEXT);

  if (calcMenuSelected < calcMenuTop) calcMenuTop = calcMenuSelected;
  if (calcMenuSelected >= calcMenuTop + CALC_MENU_VISIBLE) {
    calcMenuTop = calcMenuSelected - CALC_MENU_VISIBLE + 1;
  }
  int maxTop = max(0, CALC_MENU_ITEMS - CALC_MENU_VISIBLE);
  if (calcMenuTop > maxTop) calcMenuTop = maxTop;

  for (int row = 0; row < CALC_MENU_VISIBLE; row++) {
    int i = calcMenuTop + row;
    if (i >= CALC_MENU_ITEMS) break;
    int y = 27 + row * 17;
    uint16_t c = COLOR_GRAY;
    if (i == calcMenuSelected) {
      tft.fillRoundRect(4, y - 7, 152, 16, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    tft.setTextSize(1);
    tft.setTextColor(c);
    tft.setCursor(8, y - 1);
    tft.print(i + 1);
    tft.print(".");
    uiTextSmall(22, y + 5, calcMenuItemText(i), c);
  }

  if (calcMenuTop > 0) tft.fillTriangle(151, 20, 155, 20, 153, 23, COLOR_YELLOW);
  if (calcMenuTop + CALC_MENU_VISIBLE < CALC_MENU_ITEMS) {
    tft.fillTriangle(151, 103, 155, 103, 153, 106, COLOR_YELLOW);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(6, 123, T("2/8 ВЫБОР A ОТКР D НАЗ", "2/8 MOVE A OPEN D BACK"), COLOR_GRAY);
}

void calculationsMenuKey(char key) {
  if (key == '2') {
    calcMenuSelected--;
    if (calcMenuSelected < 0) calcMenuSelected = CALC_MENU_ITEMS - 1;
    drawCalculationsMenu();
    return;
  }
  if (key == '8') {
    calcMenuSelected++;
    if (calcMenuSelected >= CALC_MENU_ITEMS) calcMenuSelected = 0;
    drawCalculationsMenu();
    return;
  }
  if (key == 'A' || key == '#') {
    if (calcMenuSelected == 0) {
      currentScreen = SCREEN_CALCULATOR;
      calcInput = ""; calcValue = 0; calcOperator = 0;
      calcHasFirstValue = false; calcResultShown = false;
      drawCalculator();
    } else if (calcMenuSelected == 1) {
      currentScreen = SCREEN_PROGRAMMER_CALC;
      progInput = ""; progValue = 0; progFirst = 0;
      progOperation = PROG_OP_NONE; progWaitingSecond = false; progResultShown = false;
      drawProgrammerCalculator();
    } else if (calcMenuSelected == 2) {
      truthOpSelected = 0;
      currentScreen = SCREEN_TRUTH_TABLE;
      drawTruthTable();
    } else if (calcMenuSelected == 3) {
      currentScreen = SCREEN_MATH_CALC;
      mathInput = ""; mathValue = 0.0; mathResultShown = false;
      drawMathCalculator();
    } else if (calcMenuSelected == 4) {
      currentScreen = SCREEN_FUNCTION_GRAPH;
      graphFunctionSelected = 0;
      graphZoomIndex = 2;
      graphGridEnabled = true;
      drawFunctionGraph();
    } else if (calcMenuSelected == 5) {
      currentScreen = SCREEN_PASSWORD_GENERATOR;
      if (!generatedPassword.length()) generatePassword();
      drawPasswordGenerator();
    } else {
      currentScreen = SCREEN_HASH_CALC;
      hashInput = "";
      hashCursor = 0;
      hashInputMode = systemLanguage == LANG_RU ? INPUT_RUS_LOWER : INPUT_LAT_LOWER;
      hashMultiActive = false;
      hashShowingResult = false;
      hashSha256 = "";
      hashCrc32 = "";
      drawHashCalculator();
    }
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_MENU;
    drawMenu();
  }
}




void drawCalculator() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("КАЛЬКУЛЯТОР", "CALCULATOR"), COLOR_TEXT);
  tft.drawRoundRect(5, 25, 150, 38, 4, COLOR_GRAY);
  updateCalculatorDisplay();
  tft.setTextSize(1); tft.setTextColor(COLOR_GRAY);
  tft.setCursor(8, 70); tft.print("A+   B-   Cx   D/");
  tft.setCursor(8, 84); tft.print("#=");
  uiTextSmall(35, 91, T("* = СБРОС", "* = CLEAR"), COLOR_GRAY);
  tft.drawFastHLine(0, 105, 160, COLOR_GRAY);
  uiTextSmall(5, 122, T("УДЕРЖ * = МЕНЮ", "HOLD * = MENU"), COLOR_GRAY);
}

String formatNumber(double value) {
  String result = String(value, 4);
  while (result.endsWith("0")) result.remove(result.length() - 1);
  if (result.endsWith(".")) result.remove(result.length() - 1);
  return result;
}

void updateCalculatorDisplay() {
  tft.fillRect(8, 28, 144, 32, COLOR_BG);
  String value;
  if (calcInput.length()) value = calcInput;
  else if (calcHasFirstValue) value = formatNumber(calcValue);
  else value = "0";
  if (value.length() > 11) value = value.substring(value.length() - 11);

  tft.setTextSize(2); tft.setTextColor(COLOR_GREEN, COLOR_BG);
  int width = value.length() * 12;
  int x = 148 - width; if (x < 10) x = 10;
  tft.setCursor(x, 36); tft.print(value);
  if (calcOperator != 0) {
    tft.setTextSize(1); tft.setTextColor(COLOR_YELLOW, COLOR_BG);
    tft.setCursor(11, 50); tft.print(calcOperator);
  }
}

double calculateResult(double a, double b, char op) {
  if (op == '+') return a + b;
  if (op == '-') return a - b;
  if (op == '*') return a * b;
  if (op == '/' && b != 0) return a / b;
  return 0;
}

void calculatorKey(char key) {
  if (key >= '0' && key <= '9') {
    if (calcResultShown) {
      calcInput = ""; calcValue = 0; calcOperator = 0;
      calcHasFirstValue = false; calcResultShown = false;
    }
    if (calcInput.length() < 10) calcInput += key;
    updateCalculatorDisplay();
    return;
  }

  char newOperator = 0;
  if (key == 'A') newOperator = '+';
  else if (key == 'B') newOperator = '-';
  else if (key == 'C') newOperator = '*';
  else if (key == 'D') newOperator = '/';

  if (newOperator) {
    if (calcResultShown) {
      calcValue = calcInput.toDouble();
      calcInput = "";
      calcHasFirstValue = true;
      calcResultShown = false;
      calcOperator = newOperator;
      updateCalculatorDisplay();
      return;
    }
    if (!calcHasFirstValue && calcInput.length()) {
      calcValue = calcInput.toDouble();
      calcInput = "";
      calcHasFirstValue = true;
    } else if (calcHasFirstValue && calcOperator && calcInput.length()) {
      calcValue = calculateResult(calcValue, calcInput.toDouble(), calcOperator);
      calcInput = "";
    }
    calcOperator = newOperator;
    updateCalculatorDisplay();
    return;
  }

  if (key == '#') {
    if (calcHasFirstValue && calcOperator && calcInput.length()) {
      calcValue = calculateResult(calcValue, calcInput.toDouble(), calcOperator);
      calcInput = formatNumber(calcValue);
      calcOperator = 0;
      calcHasFirstValue = true;
      calcResultShown = true;
      updateCalculatorDisplay();
    }
    return;
  }

  if (key == '*') {
    calcInput = ""; calcValue = 0; calcOperator = 0;
    calcHasFirstValue = false; calcResultShown = false;
    updateCalculatorDisplay();
  }
}




String binary16(uint16_t value) {
  String out = "";
  for (int bit = 15; bit >= 0; bit--) out += (value & (1U << bit)) ? '1' : '0';
  return out;
}

uint16_t programmerCurrentValue() {
  if (progInput.length()) {
    unsigned long v = strtoul(progInput.c_str(), nullptr, 10);
    if (v > 65535UL) v = 65535UL;
    return (uint16_t)v;
  }
  return progValue;
}

String programmerOperationName(int index) {
  static const char* names[PROG_OP_COUNT] = { "AND", "NAND", "OR", "NOR", "XOR", "XNOR", "NOT", "BUFFER" };
  if (index < 0 || index >= PROG_OP_COUNT) return "";
  return String(names[index]);
}

String programmerOperationDescription(int index) {
  if (systemLanguage == LANG_EN) {
    static const char* d[PROG_OP_COUNT] = { "bitwise AND", "NOT AND", "bitwise OR", "NOT OR", "exclusive OR", "NOT XOR", "bitwise NOT", "repeater" };
    return String(d[index]);
  }
  static const char* d[PROG_OP_COUNT] = { "побитовое И", "НЕ-И", "побитовое ИЛИ", "НЕ-ИЛИ", "искл. ИЛИ", "НЕ-искл. ИЛИ", "побитовое НЕ", "повторитель" };
  return String(d[index]);
}

void programmerUpdateDisplay() {
  uint16_t v = programmerCurrentValue();
  tft.fillRect(4, 22, 152, 78, COLOR_BG);

  uiTextSmall(7, 33, progWaitingSecond ? T("ВТОРОЕ ЧИСЛО B", "SECOND VALUE B") : T("ВВОД DEC / ЧИСЛО A", "DEC INPUT / VALUE A"), COLOR_GRAY);

  char decBuf[8]; snprintf(decBuf, sizeof(decBuf), "%u", (unsigned)v);
  char hexBuf[8]; snprintf(hexBuf, sizeof(hexBuf), "%04X", (unsigned)v);
  char octBuf[10]; snprintf(octBuf, sizeof(octBuf), "%06o", (unsigned)v);

  tft.setTextSize(1); tft.setTextColor(COLOR_GREEN, COLOR_BG);
  tft.setCursor(7, 43); tft.print("DEC "); tft.print(decBuf);
  tft.setCursor(82, 43); tft.print("HEX "); tft.print(hexBuf);
  tft.setCursor(7, 56); tft.print("OCT "); tft.print(octBuf);

  tft.setTextColor(COLOR_YELLOW, COLOR_BG);
  tft.setCursor(7, 69); tft.print("BIN");
  tft.setCursor(7, 81); tft.print(binary16(v));

  if (progOperation != PROG_OP_NONE) {
    int opIndex = (int)progOperation - 1;
    uiTextSmall(7, 98, T("ОП:", "OP:"), COLOR_ACCENT);
    tft.setTextSize(1); tft.setTextColor(COLOR_ACCENT, COLOR_BG); tft.setCursor(28, 92);
    tft.print(programmerOperationName(opIndex));
  }
}
void drawProgrammerCalculator() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiTextSmall(5, 13, T("ПРОГРАММИСТСКИЙ + КОНВ.", "PROGRAMMER + CONVERTER"), COLOR_TEXT);
  programmerUpdateDisplay();
  tft.drawFastHLine(0, 104, 160, COLOR_GRAY);
  uiTextSmall(3, 114, T("A ВЫБРАТЬ ОП.   # СЧИТАТЬ", "A CHOOSE OP.   # CALC"), COLOR_GRAY);
  uiTextSmall(3, 124, T("B УДАЛ C СБРОС D НАЗ", "B DEL C CLEAR D BACK"), COLOR_GRAY);
}

void programmerApplyOperation(int index) {
  uint16_t a = programmerCurrentValue();
  progInput = "";
  progResultShown = false;


  if (index == 6) {
    progValue = (uint16_t)(~a);
    progOperation = PROG_OP_NONE;
    progWaitingSecond = false;
    progResultShown = true;
    return;
  }
  if (index == 7) {
    progValue = a;
    progOperation = PROG_OP_NONE;
    progWaitingSecond = false;
    progResultShown = true;
    return;
  }

  progFirst = a;
  progOperation = (ProgrammerOperation)(index + 1);
  progWaitingSecond = true;
  progValue = 0;
}

void drawProgrammerOperations() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ЛОГИЧЕСКАЯ ОПЕРАЦИЯ", "LOGIC OPERATION"), COLOR_TEXT);

  if (progOpSelected < progOpTop) progOpTop = progOpSelected;
  if (progOpSelected >= progOpTop + PROG_OP_VISIBLE) progOpTop = progOpSelected - PROG_OP_VISIBLE + 1;

  for (int row = 0; row < PROG_OP_VISIBLE; row++) {
    int i = progOpTop + row; if (i >= PROG_OP_COUNT) break;
    int y = 26 + row * 16;
    uint16_t c = COLOR_GRAY;
    if (i == progOpSelected) {
      tft.fillRoundRect(4, y - 3, 152, 15, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    tft.setTextSize(1); tft.setTextColor(c); tft.setCursor(8, y); tft.print(programmerOperationName(i));
    uiTextSmall(54, y + 6, programmerOperationDescription(i), c);
  }
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(4, 123, T("2/8 ВЫБОР A ОК D НАЗ", "2/8 MOVE A OK D BACK"), COLOR_GRAY);
}

void programmerOperationsKey(char key) {
  if (key == '2') {
    progOpSelected--; if (progOpSelected < 0) progOpSelected = PROG_OP_COUNT - 1;
    drawProgrammerOperations(); return;
  }
  if (key == '8') {
    progOpSelected++; if (progOpSelected >= PROG_OP_COUNT) progOpSelected = 0;
    drawProgrammerOperations(); return;
  }
  if (key == 'A' || key == '#') {
    programmerApplyOperation(progOpSelected);
    currentScreen = SCREEN_PROGRAMMER_CALC;
    drawProgrammerCalculator();
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_PROGRAMMER_CALC;
    drawProgrammerCalculator();
  }
}

void programmerCalculatorKey(char key) {
  if (key >= '0' && key <= '9') {
    if (progResultShown && !progWaitingSecond) {
      progInput = ""; progValue = 0; progResultShown = false;
    }
    if (progInput.length() < 5) {
      String next = progInput + key;
      if (strtoul(next.c_str(), nullptr, 10) <= 65535UL) progInput = next;
    }
    programmerUpdateDisplay(); return;
  }
  if (key == 'A') {

    progOpSelected = 0; progOpTop = 0;
    currentScreen = SCREEN_PROGRAMMER_OPS;
    drawProgrammerOperations(); return;
  }
  if (key == 'B') {
    if (progInput.length()) progInput.remove(progInput.length() - 1);
    programmerUpdateDisplay(); return;
  }
  if (key == 'C') {
    progInput = ""; progValue = 0; progFirst = 0; progOperation = PROG_OP_NONE;
    progWaitingSecond = false; progResultShown = false;
    programmerUpdateDisplay(); return;
  }
  if (key == '#') {
    if (progWaitingSecond && progOperation != PROG_OP_NONE && progInput.length()) {
      uint16_t b = programmerCurrentValue();
      uint16_t r = 0;
      switch (progOperation) {
        case PROG_OP_AND: r = progFirst & b; break;
        case PROG_OP_NAND: r = (uint16_t)~(progFirst & b); break;
        case PROG_OP_OR: r = progFirst | b; break;
        case PROG_OP_NOR: r = (uint16_t)~(progFirst | b); break;
        case PROG_OP_XOR: r = progFirst ^ b; break;
        case PROG_OP_XNOR: r = (uint16_t)~(progFirst ^ b); break;
        default: r = b; break;
      }
      progValue = r; progInput = ""; progOperation = PROG_OP_NONE;
      progWaitingSecond = false; progResultShown = true;
      programmerUpdateDisplay();
    }
    return;
  }
  if (key == 'D') {
    currentScreen = SCREEN_CALCULATIONS_MENU;
    drawCalculationsMenu();
  }
}




int truthGateResult(int opIndex, int a, int b) {
  a = a ? 1 : 0;
  b = b ? 1 : 0;
  switch (opIndex) {
    case 0: return a & b;
    case 1: return !(a & b);
    case 2: return a | b;
    case 3: return !(a | b);
    case 4: return a ^ b;
    case 5: return !(a ^ b);
    case 6: return !a;
    case 7: return a;
  }
  return 0;
}

void drawTruthTable() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiTextSmall(5, 13, T("ТАБЛИЦА ИСТИННОСТИ", "TRUTH TABLE"), COLOR_TEXT);

  uiTextSmall(7, 31, T("ОПЕРАЦИЯ:", "GATE:"), COLOR_GRAY);
  tft.setTextSize(1); tft.setTextColor(COLOR_YELLOW, COLOR_BG);
  tft.setCursor(76, 24); tft.print(programmerOperationName(truthOpSelected));

  bool unary = truthOpSelected >= 6;
  if (unary) {
    tft.setTextSize(2); tft.setTextColor(COLOR_ACCENT, COLOR_BG);
    tft.setCursor(42, 43); tft.print("A");
    tft.setCursor(108, 43); tft.print("Y");
    tft.drawFastHLine(24, 61, 118, COLOR_GRAY);

    for (int a = 0; a <= 1; a++) {
      int y = 70 + a * 22;
      tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.setCursor(42, y); tft.print(a);
      tft.setTextColor(COLOR_GREEN, COLOR_BG); tft.setCursor(108, y); tft.print(truthGateResult(truthOpSelected, a));
    }
  } else {
    tft.setTextSize(1); tft.setTextColor(COLOR_ACCENT, COLOR_BG);
    tft.setCursor(30, 43); tft.print("A");
    tft.setCursor(70, 43); tft.print("B");
    tft.setCursor(118, 43); tft.print("Y");
    tft.drawFastHLine(18, 55, 125, COLOR_GRAY);

    int row = 0;
    for (int a = 0; a <= 1; a++) {
      for (int b = 0; b <= 1; b++) {
        int y = 62 + row * 12;
        tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.setCursor(30, y); tft.print(a);
        tft.setCursor(70, y); tft.print(b);
        tft.setTextColor(COLOR_GREEN, COLOR_BG); tft.setCursor(118, y); tft.print(truthGateResult(truthOpSelected, a, b));
        row++;
      }
    }
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(4, 123, T("2/8 ОПЕРАЦИЯ   D НАЗАД", "2/8 GATE      D BACK"), COLOR_GRAY);
}

void truthTableKey(char key) {
  if (key == '2') {
    truthOpSelected--;
    if (truthOpSelected < 0) truthOpSelected = PROG_OP_COUNT - 1;
    drawTruthTable();
    return;
  }
  if (key == '8') {
    truthOpSelected++;
    if (truthOpSelected >= PROG_OP_COUNT) truthOpSelected = 0;
    drawTruthTable();
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_CALCULATIONS_MENU;
    drawCalculationsMenu();
  }
}




String formatMathNumber(double value) {
  if (isnan(value) || isinf(value)) return T("ОШИБКА", "ERROR");
  if (fabs(value) < 0.0000001) value = 0.0;
  String s = String(value, 6);
  while (s.endsWith("0")) s.remove(s.length() - 1);
  if (s.endsWith(".")) s.remove(s.length() - 1);
  if (s.length() > 15) s = String(value, 6);
  return s;
}

String mathFunctionName(int index) {
  static const char* names[MATH_FUNCTION_COUNT] = {
    "SIN", "COS", "TAN", "SQRT", "LN", "LOG10", "EXP", "x^2", "x^3", "1/x", "ABS", "PI", "e"
  };
  if (index < 0 || index >= MATH_FUNCTION_COUNT) return "";
  return String(names[index]);
}

String mathFunctionDescription(int index) {
  if (systemLanguage == LANG_EN) {
    static const char* d[MATH_FUNCTION_COUNT] = {
      "sine", "cosine", "tangent", "square root", "natural log", "decimal log", "exponential", "square", "cube", "reciprocal", "absolute", "constant pi", "Euler number"
    };
    return String(d[index]);
  }
  static const char* d[MATH_FUNCTION_COUNT] = {
    "синус", "косинус", "тангенс", "корень", "нат. логарифм", "дес. логарифм", "экспонента", "квадрат", "куб", "обратное", "модуль", "число пи", "число Эйлера"
  };
  return String(d[index]);
}

void mathUpdateDisplay() {
  tft.fillRect(5, 23, 150, 69, COLOR_BG);
  tft.drawRoundRect(5, 24, 150, 35, 4, COLOR_GRAY);
  String shown = mathInput.length() ? mathInput : formatMathNumber(mathValue);
  if (!shown.length()) shown = "0";

  tft.setTextSize(shown.length() > 10 ? 1 : 2);
  tft.setTextColor(COLOR_GREEN, COLOR_BG);
  int cw = shown.length() > 10 ? 6 : 12;
  int x = 149 - (int)shown.length() * cw; if (x < 9) x = 9;
  tft.setCursor(x, shown.length() > 10 ? 38 : 34); tft.print(shown);
  uiTextSmall(8, 72, mathDegrees ? "DEG" : "RAD", COLOR_YELLOW);
  uiTextSmall(42, 72, T("A ФУНКЦИИ", "A FUNCTIONS"), COLOR_GRAY);
  uiTextSmall(8, 87, T("* ТОЧКА  C +/-  # DEG/RAD", "* DOT C +/- # DEG/RAD"), COLOR_GRAY);
}

void drawMathCalculator() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiTextSmall(5, 13, T("МАТЕМАТИЧЕСКИЙ КАЛЬК.", "SCIENTIFIC CALC"), COLOR_TEXT);
  mathUpdateDisplay();
  tft.drawFastHLine(0, 105, 160, COLOR_GRAY);
  uiTextSmall(3, 115, T("B УДАЛ  A ФУНКЦИИ", "B DEL A FUNCTIONS"), COLOR_GRAY);
  uiTextSmall(3, 125, T("D НАЗАД", "D BACK"), COLOR_GRAY);
}

void drawMathFunctions() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("МАТ. ФУНКЦИИ", "MATH FUNCTIONS"), COLOR_TEXT);

  if (mathFunctionSelected < mathFunctionTop) mathFunctionTop = mathFunctionSelected;
  if (mathFunctionSelected >= mathFunctionTop + MATH_FUNCTION_VISIBLE) mathFunctionTop = mathFunctionSelected - MATH_FUNCTION_VISIBLE + 1;

  for (int row = 0; row < MATH_FUNCTION_VISIBLE; row++) {
    int i = mathFunctionTop + row; if (i >= MATH_FUNCTION_COUNT) break;
    int y = 26 + row * 16;
    uint16_t c = COLOR_GRAY;
    if (i == mathFunctionSelected) {
      tft.fillRoundRect(4, y - 3, 152, 15, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    tft.setTextSize(1); tft.setTextColor(c); tft.setCursor(7, y); tft.print(mathFunctionName(i));
    uiTextSmall(45, y + 6, mathFunctionDescription(i), c);
  }
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(4, 123, T("2/8 ВЫБОР A ОК D НАЗ", "2/8 MOVE A OK D BACK"), COLOR_GRAY);
}

void mathApplyFunction(int index) {
  double x = mathInput.length() ? mathInput.toDouble() : mathValue;
  double r = x;
  bool ok = true;
  double angle = mathDegrees ? x * PI / 180.0 : x;

  switch (index) {
    case 0: r = sin(angle); break;
    case 1: r = cos(angle); break;
    case 2: r = tan(angle); break;
    case 3: if (x < 0) ok = false; else r = sqrt(x); break;
    case 4: if (x <= 0) ok = false; else r = log(x); break;
    case 5: if (x <= 0) ok = false; else r = log10(x); break;
    case 6: r = exp(x); break;
    case 7: r = x * x; break;
    case 8: r = x * x * x; break;
    case 9: if (x == 0) ok = false; else r = 1.0 / x; break;
    case 10: r = fabs(x); break;
    case 11: r = PI; break;
    case 12: r = exp(1.0); break;
  }

  if (!ok || isnan(r) || isinf(r)) {
    mathInput = ""; mathValue = NAN; mathResultShown = true;
  } else {
    mathValue = r; mathInput = ""; mathResultShown = true;
  }
}

void mathFunctionsKey(char key) {
  if (key == '2') {
    mathFunctionSelected--; if (mathFunctionSelected < 0) mathFunctionSelected = MATH_FUNCTION_COUNT - 1;
    drawMathFunctions(); return;
  }
  if (key == '8') {
    mathFunctionSelected++; if (mathFunctionSelected >= MATH_FUNCTION_COUNT) mathFunctionSelected = 0;
    drawMathFunctions(); return;
  }
  if (key == 'A' || key == '#') {
    mathApplyFunction(mathFunctionSelected);
    currentScreen = SCREEN_MATH_CALC;
    drawMathCalculator(); return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_MATH_CALC;
    drawMathCalculator();
  }
}

void mathCalculatorKey(char key) {
  if (key >= '0' && key <= '9') {
    if (mathResultShown) { mathInput = ""; mathValue = 0.0; mathResultShown = false; }
    if (mathInput.length() < 14) mathInput += key;
    mathUpdateDisplay(); return;
  }
  if (key == '*') {
    if (mathResultShown) { mathInput = "0"; mathValue = 0.0; mathResultShown = false; }
    if (!mathInput.length()) mathInput = "0";
    if (mathInput.indexOf('.') < 0 && mathInput.length() < 14) mathInput += '.';
    mathUpdateDisplay(); return;
  }
  if (key == 'B') {
    if (mathInput.length()) mathInput.remove(mathInput.length() - 1);
    else if (mathResultShown) { mathValue = 0.0; mathResultShown = false; }
    mathUpdateDisplay(); return;
  }
  if (key == 'C') {
    if (mathInput.length()) {
      if (mathInput.startsWith("-")) mathInput.remove(0, 1);
      else mathInput = "-" + mathInput;
    } else {
      mathValue = -mathValue;
      mathResultShown = true;
    }
    mathUpdateDisplay(); return;
  }
  if (key == '#') {
    mathDegrees = !mathDegrees;
    mathUpdateDisplay(); return;
  }
  if (key == 'A') {
    mathFunctionSelected = 0; mathFunctionTop = 0;
    currentScreen = SCREEN_MATH_FUNCTIONS;
    drawMathFunctions(); return;
  }
  if (key == 'D') {
    currentScreen = SCREEN_CALCULATIONS_MENU;
    drawCalculationsMenu();
  }
}




String graphFunctionName(int index) {
  static const char* names[GRAPH_FUNCTION_COUNT] = {
    "sin(x)", "cos(x)", "tan(x)", "x", "x^2", "x^3", "sqrt(x)", "1/x", "ln(x)", "exp(x)"
  };
  if (index < 0 || index >= GRAPH_FUNCTION_COUNT) return "";
  return String(names[index]);
}

String graphCurveName(int index) {
  if (index < 0 || index >= GRAPH_FUNCTION_COUNT) return "";
  if (systemLanguage == LANG_EN) {
    static const char* names[GRAPH_FUNCTION_COUNT] = {
      "SINE WAVE", "COSINE WAVE", "TANGENT CURVE", "STRAIGHT LINE", "PARABOLA",
      "CUBIC PARABOLA", "PARABOLA BRANCH", "HYPERBOLA", "LOG CURVE", "EXPONENTIAL"
    };
    return String(names[index]);
  }
  static const char* names[GRAPH_FUNCTION_COUNT] = {
    "СИНУСОИДА", "КОСИНУСОИДА", "ГРАФИК ТАНГЕНСА", "ПРЯМАЯ", "ПАРАБОЛА",
    "КУБИЧ. ПАРАБОЛА", "ВЕТВЬ ПАРАБОЛЫ", "ГИПЕРБОЛА", "ЛОГАРИФМ. КРИВАЯ", "ЭКСПОНЕНТА"
  };
  return String(names[index]);
}

double graphEvaluate(int index, double x, bool& valid) {
  valid = true;
  double y = 0.0;
  switch (index) {
    case 0: y = sin(x); break;
    case 1: y = cos(x); break;
    case 2: y = tan(x); break;
    case 3: y = x; break;
    case 4: y = x * x; break;
    case 5: y = x * x * x; break;
    case 6: if (x < 0) valid = false; else y = sqrt(x); break;
    case 7: if (fabs(x) < 0.000001) valid = false; else y = 1.0 / x; break;
    case 8: if (x <= 0) valid = false; else y = log(x); break;
    case 9: y = exp(x); break;
    default: valid = false; break;
  }
  if (!valid || isnan(y) || isinf(y) || fabs(y) > 1000000.0) { valid = false; return 0.0; }
  return y;
}

void graphYBounds(int index, double xHalf, double& yMin, double& yMax) {
  switch (index) {
    case 0: case 1: yMin = -1.25; yMax = 1.25; break;
    case 2: yMin = -5.0; yMax = 5.0; break;
    case 3: yMin = -xHalf; yMax = xHalf; break;
    case 4: yMin = 0.0; yMax = xHalf * xHalf; break;
    case 5: yMin = -xHalf * xHalf * xHalf; yMax = xHalf * xHalf * xHalf; break;
    case 6: yMin = 0.0; yMax = sqrt(max(0.01, xHalf)); break;
    case 7: yMin = -5.0; yMax = 5.0; break;
    case 8: yMin = -4.0; yMax = max(1.0, log(max(1.01, xHalf))); break;
    case 9: yMin = 0.0; yMax = min(20.0, exp(min(3.0, xHalf))); break;
    default: yMin = -5.0; yMax = 5.0; break;
  }
  if (fabs(yMax - yMin) < 0.000001) { yMin -= 1.0; yMax += 1.0; }
}

void drawFunctionGraph() {
  const int gx = 4, gy = 21, gw = 152, gh = 82;
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  String formulaTitle = String("y = ") + graphFunctionName(graphFunctionSelected);
  uiTextSmall(4, 7, formulaTitle, COLOR_TEXT);
  uiTextSmall(4, 15, graphCurveName(graphFunctionSelected), COLOR_YELLOW);

  double xHalf = graphHalfRanges[graphZoomIndex];
  double yMin, yMax;
  graphYBounds(graphFunctionSelected, xHalf, yMin, yMax);

  tft.drawRect(gx, gy, gw, gh, COLOR_GRAY);
  if (graphGridEnabled) {
    for (int k = 1; k < 4; k++) {
      int xx = gx + (gw - 1) * k / 4;
      int yy = gy + (gh - 1) * k / 4;
      for (int p = gy + 1; p < gy + gh - 1; p += 3) tft.drawPixel(xx, p, COLOR_GRAY);
      for (int p = gx + 1; p < gx + gw - 1; p += 3) tft.drawPixel(p, yy, COLOR_GRAY);
    }
  }


  int axisX = gx + (gw - 1) / 2;
  tft.drawFastVLine(axisX, gy + 1, gh - 2, COLOR_ACCENT);
  if (yMin <= 0.0 && yMax >= 0.0) {
    int axisY = gy + (int)round((yMax / (yMax - yMin)) * (gh - 1));
    axisY = constrain(axisY, gy + 1, gy + gh - 2);
    tft.drawFastHLine(gx + 1, axisY, gw - 2, COLOR_ACCENT);
  }

  bool prevValid = false;
  int prevX = 0, prevY = 0;
  for (int px = 0; px < gw - 2; px++) {
    double x = -xHalf + (2.0 * xHalf * px) / (double)(gw - 3);
    bool valid;
    double y = graphEvaluate(graphFunctionSelected, x, valid);
    if (!valid || y < yMin || y > yMax) { prevValid = false; continue; }
    int sy = gy + (int)round((yMax - y) * (gh - 1) / (yMax - yMin));
    int sx = gx + 1 + px;
    sy = constrain(sy, gy + 1, gy + gh - 2);
    if (prevValid && abs(sy - prevY) < gh / 2) tft.drawLine(prevX, prevY, sx, sy, COLOR_YELLOW);
    else tft.drawPixel(sx, sy, COLOR_YELLOW);
    prevValid = true; prevX = sx; prevY = sy;
  }

  char rangeBuf[24];
  snprintf(rangeBuf, sizeof(rangeBuf), "X: -%.2g..%.2g", xHalf, xHalf);
  uiTextSmall(5, 111, String(rangeBuf), COLOR_GREEN);
  uiTextSmall(88, 111, graphGridEnabled ? T("СЕТКА", "GRID") : T("БЕЗ СЕТКИ", "NO GRID"), COLOR_GRAY);
  uiTextSmall(2, 120, T("2/8 ФУНК 4/6 МАСШТ", "2/8 FUNC 4/6 ZOOM"), COLOR_GRAY);
  uiTextSmall(2, 127, T("# СЕТКА A СБРОС D НАЗ", "# GRID A RESET D BACK"), COLOR_GRAY);
}

void functionGraphKey(char key) {
  if (key == '2') {
    graphFunctionSelected--;
    if (graphFunctionSelected < 0) graphFunctionSelected = GRAPH_FUNCTION_COUNT - 1;
    drawFunctionGraph(); return;
  }
  if (key == '8') {
    graphFunctionSelected++;
    if (graphFunctionSelected >= GRAPH_FUNCTION_COUNT) graphFunctionSelected = 0;
    drawFunctionGraph(); return;
  }
  if (key == '4') {
    if (graphZoomIndex > 0) graphZoomIndex--;
    drawFunctionGraph(); return;
  }
  if (key == '6') {
    if (graphZoomIndex < GRAPH_ZOOM_COUNT - 1) graphZoomIndex++;
    drawFunctionGraph(); return;
  }
  if (key == '#') { graphGridEnabled = !graphGridEnabled; drawFunctionGraph(); return; }
  if (key == 'A') { graphZoomIndex = 2; graphGridEnabled = true; drawFunctionGraph(); return; }
  if (key == 'D' || key == 'B') { currentScreen = SCREEN_CALCULATIONS_MENU; drawCalculationsMenu(); }
}




void drawClockScreen() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("ВРЕМЯ КАЛИНИНГРАДА", "KALININGRAD TIME"), COLOR_TEXT);
  tft.drawRoundRect(5, 28, 150, 55, 5, COLOR_ACCENT);
  tft.setTextSize(1); tft.setTextColor(COLOR_GRAY);
  tft.setCursor(53, 89); tft.print("UTC +02:00");
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(5, 123, T("B НАЗАД", "B BACK"), COLOR_GRAY);
  oldTimeString = "        "; oldDateString = "          ";
  updateClock(true);
}

void drawChangedString(String newValue, String& oldValue, int x, int y, int size, uint16_t color) {
  int charWidth = 6 * size;
  int charHeight = 8 * size;
  while (oldValue.length() < newValue.length()) oldValue += ' ';
  for (int i = 0; i < newValue.length(); i++) {
    if (oldValue[i] != newValue[i]) {
      int px = x + i * charWidth;
      tft.fillRect(px, y, charWidth, charHeight, COLOR_BG);
      tft.setTextSize(size); tft.setTextColor(color, COLOR_BG);
      tft.setCursor(px, y); tft.print(newValue[i]);
    }
  }
  oldValue = newValue;
}

void configureKaliningradTime() {
  configTime(2 * 3600, 0, "pool.ntp.org", "time.google.com", "time.cloudflare.com");
  ntpConfigured = true;
}

void updateClock(bool forceUpdate) {
  if (!forceUpdate && millis() - lastClockUpdate < 200) return;
  lastClockUpdate = millis();
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 20)) {
    tft.fillRect(8, 38, 144, 25, COLOR_BG);
    uiText(44, 54, WiFi.status() == WL_CONNECTED ? T("СИНХРОНИЗАЦИЯ", "SYNCING") : T("НЕТ WI-FI", "NO WI-FI"), COLOR_YELLOW);
    return;
  }
  char tb[9]; strftime(tb, sizeof(tb), "%H:%M:%S", &timeinfo);
  drawChangedString(String(tb), oldTimeString, 32, 43, 2, COLOR_GREEN);
  char db[11]; strftime(db, sizeof(db), "%d.%m.%Y", &timeinfo);
  drawChangedString(String(db), oldDateString, 50, 68, 1, COLOR_TEXT);
}





String formatTimerMs(unsigned long ms) {
  unsigned long sec = (ms + 999UL) / 1000UL;
  unsigned long h = sec / 3600UL;
  unsigned long m = (sec / 60UL) % 60UL;
  unsigned long ss = sec % 60UL;
  char buf[9];
  snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu", h > 99 ? 99 : h, m, ss);
  return String(buf);
}

unsigned long timerDigitsToMs(const String& digits) {
  String d = digits;
  while (d.length() < 6) d = "0" + d;
  if (d.length() > 6) d = d.substring(d.length() - 6);
  int h = d.substring(0, 2).toInt();
  int m = d.substring(2, 4).toInt();
  int ss = d.substring(4, 6).toInt();
  if (m > 59) m = 59;
  if (ss > 59) ss = 59;
  return ((unsigned long)h * 3600UL + (unsigned long)m * 60UL + (unsigned long)ss) * 1000UL;
}

void drawTimerScreen() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("ДАТА И ВРЕМЯ / ТАЙМЕР", "DATE & TIME / TIMER"), COLOR_TEXT);
  tft.drawRoundRect(8, 30, 144, 43, 5, COLOR_ACCENT);
  oldTimerString = "        "; oldTimerStatus = ""; updateTimer(true);
  uiTextSmall(8, 84, T("ВВОД ЧЧММСС (130 = 1:30)", "ENTER HHMMSS (130=1:30)"), COLOR_GRAY);
  uiTextSmall(8, 97, T("A СТАРТ/ПАУЗА   B СБРОС", "A START/PAUSE   B RESET"), COLOR_GRAY);
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(5, 123, T("C +1 МИН        D НАЗАД", "C +1 MIN        D BACK"), COLOR_GRAY);
}

void updateTimer(bool forceUpdate) {
  if (!forceUpdate && millis() - lastTimerUiUpdate < 100) return;
  lastTimerUiUpdate = millis();

  if (timerRunning) {
    int32_t delta = (int32_t)(timerEndMs - millis());
    if (delta <= 0) {
      timerRunning = false;
      timerRemainingMs = 0;
      timerDone = true;
    } else timerRemainingMs = (unsigned long)delta;
  } else if (!timerInputDigits.length() && timerRemainingMs == 0 && !timerDone) {
    timerRemainingMs = 0;
  }

  unsigned long shownMs = timerRunning || timerRemainingMs ? timerRemainingMs : timerDigitsToMs(timerInputDigits);
  String value = formatTimerMs(shownMs);
  drawChangedString(value, oldTimerString, 32, 44, 2, timerDone ? COLOR_YELLOW : COLOR_GREEN);


  String newStatus = "";
  uint16_t statusColor = COLOR_GRAY;
  if (timerDone) {
    newStatus = T("ГОТОВО", "DONE");
    statusColor = COLOR_YELLOW;
  } else if (timerRunning) {
    newStatus = T("ИДЁТ", "RUNNING");
    statusColor = COLOR_GREEN;
  } else if (timerRemainingMs > 0) {
    newStatus = T("ПАУЗА", "PAUSED");
    statusColor = COLOR_YELLOW;
  }

  if (forceUpdate || newStatus != oldTimerStatus) {


    tft.fillRect(38, 56, 84, 18, COLOR_BG);
    if (newStatus.length()) {
      int w = utf8Count(newStatus) * 6;
      int x = (160 - w) / 2;
      uiText(x, 70, newStatus, statusColor);
    }
    oldTimerStatus = newStatus;
  }
}

void timerKey(char key) {
  if (key >= '0' && key <= '9' && !timerRunning) {
    if (timerRemainingMs > 0 || timerDone) {
      timerRemainingMs = 0; timerDone = false; timerInputDigits = "";
    }
    if (timerInputDigits.length() < 6) timerInputDigits += key;
    oldTimerString = "        "; updateTimer(true); return;
  }

  if (key == 'A' || key == '#') {
    if (timerRunning) {
      int32_t delta = (int32_t)(timerEndMs - millis());
      timerRemainingMs = delta > 0 ? (unsigned long)delta : 0;
      timerRunning = false;
    } else {
      if (timerRemainingMs == 0) timerRemainingMs = timerDigitsToMs(timerInputDigits);
      if (timerRemainingMs > 0) {
        timerEndMs = millis() + timerRemainingMs;
        timerRunning = true;
        timerDone = false;
        timerInputDigits = "";
      }
    }
    updateTimer(true); return;
  }

  if (key == 'B') {
    timerRunning = false; timerDone = false; timerRemainingMs = 0; timerInputDigits = "";
    oldTimerString = "        "; updateTimer(true); return;
  }

  if (key == 'C') {
    timerDone = false;
    if (timerRunning) {
      timerEndMs += 60000UL;
      timerRemainingMs += 60000UL;
    } else {
      unsigned long base = timerRemainingMs ? timerRemainingMs : timerDigitsToMs(timerInputDigits);
      timerRemainingMs = base + 60000UL;
      timerInputDigits = "";
    }
    updateTimer(true); return;
  }

  if (key == 'D') {
    currentScreen = SCREEN_DATETIME_MENU;
    drawDateTimeMenu();
  }
}




String formatStopwatchMs(unsigned long ms) {
  unsigned long tenths = (ms / 100UL) % 10UL;
  unsigned long totalSec = ms / 1000UL;
  unsigned long min = (totalSec / 60UL) % 100UL;
  unsigned long sec = totalSec % 60UL;
  char buf[8];
  snprintf(buf, sizeof(buf), "%02lu:%02lu.%1lu", min, sec, tenths);
  return String(buf);
}

void drawStopwatchScreen() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("ДАТА И ВРЕМЯ / СЕКУНДОМЕР", "DATE & TIME / STOPWATCH"), COLOR_TEXT);
  tft.drawRoundRect(8, 30, 144, 43, 5, COLOR_ACCENT);
  oldStopwatchString = "       "; updateStopwatch(true);
  uiText(33, 85, T("A СТАРТ / ПАУЗА", "A START / PAUSE"), COLOR_GRAY);
  uiText(49, 99, T("B СБРОС", "B RESET"), COLOR_GRAY);
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(5, 123, T("D НАЗАД", "D BACK"), COLOR_GRAY);
}

void updateStopwatch(bool forceUpdate) {
  if (!forceUpdate && millis() - lastStopwatchUiUpdate < 100) return;
  lastStopwatchUiUpdate = millis();
  unsigned long elapsed = stopwatchAccumMs;
  if (stopwatchRunning) elapsed += millis() - stopwatchStartMs;
  String value = formatStopwatchMs(elapsed);
  drawChangedString(value, oldStopwatchString, 38, 45, 2, COLOR_GREEN);
}

void stopwatchKey(char key) {
  if (key == 'A' || key == '#') {
    if (stopwatchRunning) {
      stopwatchAccumMs += millis() - stopwatchStartMs;
      stopwatchRunning = false;
    } else {
      stopwatchStartMs = millis();
      stopwatchRunning = true;
    }
    updateStopwatch(true); return;
  }
  if (key == 'B') {
    stopwatchRunning = false; stopwatchAccumMs = 0; stopwatchStartMs = millis();
    oldStopwatchString = "       "; updateStopwatch(true); return;
  }
  if (key == 'D') {
    currentScreen = SCREEN_DATETIME_MENU;
    drawDateTimeMenu();
  }
}




bool calendarLeap(int year) {
  return (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
}

int calendarDaysInMonth(int year, int month) {
  static const uint8_t days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  if (month == 2) return calendarLeap(year) ? 29 : 28;
  if (month < 1 || month > 12) return 30;
  return days[month - 1];
}

int calendarWeekdayMon0(int year, int month, int day) {
  static int t[] = {0,3,2,5,0,3,5,1,4,6,2,4};
  if (month < 3) year -= 1;
  int sunday0 = (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
  return (sunday0 + 6) % 7;
}

String calendarMonthName(int month) {
  static const char* ru[12] = {"ЯНВАРЬ","ФЕВРАЛЬ","МАРТ","АПРЕЛЬ","МАЙ","ИЮНЬ","ИЮЛЬ","АВГУСТ","СЕНТЯБРЬ","ОКТЯБРЬ","НОЯБРЬ","ДЕКАБРЬ"};
  static const char* en[12] = {"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"};
  if (month < 1 || month > 12) return "?";
  return String(systemLanguage == LANG_RU ? ru[month-1] : en[month-1]);
}

String calendarEventPath(int year, int month, int day) {
  char path[32];
  snprintf(path, sizeof(path), "/events/%04d%02d%02d.txt", year, month, day);
  return String(path);
}

bool calendarHasEvent(int year, int month, int day) {
  return LittleFS.exists(calendarEventPath(year, month, day));
}

String loadCalendarEvent(int year, int month, int day) {
  String path = calendarEventPath(year, month, day);
  File file = LittleFS.open(path, "r");
  if (!file) return "";
  String text = file.readString();
  file.close();
  if (text.length() > MAX_CALENDAR_EVENT_BYTES) text = text.substring(0, MAX_CALENDAR_EVENT_BYTES);
  return text;
}

bool saveCalendarEvent(int year, int month, int day, const String& text) {
  if (!LittleFS.exists("/events")) LittleFS.mkdir("/events");
  String path = calendarEventPath(year, month, day);


  if (text.length() == 0) {
    if (LittleFS.exists(path)) LittleFS.remove(path);
    return true;
  }

  File file = LittleFS.open(path, "w");
  if (!file) return false;
  size_t written = file.print(text);
  file.close();
  return written == text.length();
}

void initCalendarToToday() {
  struct tm ti;
  if (getLocalTime(&ti, 20) && ti.tm_year + 1900 >= 2020) {
    calendarYear = ti.tm_year + 1900;
    calendarMonth = ti.tm_mon + 1;
    calendarSelectedDay = ti.tm_mday;
    return;
  }

  String d = String(__DATE__);
  String mon = d.substring(0, 3);
  static const char* names[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  calendarMonth = 1;
  for (int i = 0; i < 12; i++) if (mon == names[i]) { calendarMonth = i + 1; break; }
  calendarYear = d.substring(7).toInt();
  calendarSelectedDay = d.substring(4, 6).toInt();
  if (calendarSelectedDay < 1) calendarSelectedDay = 1;
  if (calendarYear < 2020) calendarYear = 2026;
}

void calendarMoveDays(int delta) {
  while (delta > 0) {
    calendarSelectedDay++;
    if (calendarSelectedDay > calendarDaysInMonth(calendarYear, calendarMonth)) {
      calendarSelectedDay = 1;
      calendarMonth++;
      if (calendarMonth > 12) {
        calendarMonth = 1;
        if (calendarYear < 2099) calendarYear++;
        else { calendarYear = 2099; calendarMonth = 12; calendarSelectedDay = 31; return; }
      }
    }
    delta--;
  }

  while (delta < 0) {
    calendarSelectedDay--;
    if (calendarSelectedDay < 1) {
      calendarMonth--;
      if (calendarMonth < 1) {
        calendarMonth = 12;
        if (calendarYear > 2000) calendarYear--;
        else { calendarYear = 2000; calendarMonth = 1; calendarSelectedDay = 1; return; }
      }
      calendarSelectedDay = calendarDaysInMonth(calendarYear, calendarMonth);
    }
    delta++;
  }
}

void calendarMoveMonth(int delta) {
  calendarMonth += delta;
  if (calendarMonth < 1) {
    calendarMonth = 12;
    if (calendarYear > 2000) calendarYear--;
    else { calendarYear = 2000; calendarMonth = 1; }
  }
  if (calendarMonth > 12) {
    calendarMonth = 1;
    if (calendarYear < 2099) calendarYear++;
    else { calendarYear = 2099; calendarMonth = 12; }
  }
  int maxDay = calendarDaysInMonth(calendarYear, calendarMonth);
  if (calendarSelectedDay > maxDay) calendarSelectedDay = maxDay;
}

void drawCalendar() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("КАЛЕНДАРЬ", "CALENDAR"), COLOR_TEXT);

  String heading = calendarMonthName(calendarMonth) + " " + String(calendarYear);
  drawUiCentered(heading, 31, COLOR_YELLOW, u8g2_font_5x7_t_cyrillic, 5);

  static const char* weekRu[7] = {"ПН","ВТ","СР","ЧТ","ПТ","СБ","ВС"};
  static const char* weekEn[7] = {"MO","TU","WE","TH","FR","SA","SU"};
  for (int c = 0; c < 7; c++) {
    int x = 3 + c * 22;
    uiTextSmall(x, 43, String(systemLanguage == LANG_RU ? weekRu[c] : weekEn[c]), c >= 5 ? COLOR_ACCENT : COLOR_GRAY);
  }

  int first = calendarWeekdayMon0(calendarYear, calendarMonth, 1);
  int count = calendarDaysInMonth(calendarYear, calendarMonth);

  struct tm ti;
  bool haveToday = getLocalTime(&ti, 5) && ti.tm_year + 1900 >= 2020;
  int todayY = haveToday ? ti.tm_year + 1900 : -1;
  int todayM = haveToday ? ti.tm_mon + 1 : -1;
  int todayD = haveToday ? ti.tm_mday : -1;

  tft.setTextSize(1);
  for (int day = 1; day <= count; day++) {
    int cell = first + day - 1;
    int row = cell / 7;
    int col = cell % 7;
    int x = 5 + col * 22;
    int y = 50 + row * 10;

    bool today = calendarYear == todayY && calendarMonth == todayM && day == todayD;
    bool selected = day == calendarSelectedDay;
    bool hasEvent = calendarHasEvent(calendarYear, calendarMonth, day);

    if (today) tft.fillRoundRect(x - 2, y - 2, 18, 10, 2, COLOR_SELECT);
    if (selected) tft.drawRoundRect(x - 3, y - 3, 20, 12, 2, COLOR_YELLOW);

    tft.setTextColor(today ? COLOR_TEXT : (col >= 5 ? COLOR_ACCENT : COLOR_TEXT), today ? COLOR_SELECT : COLOR_BG);
    tft.setCursor(x, y);
    if (day < 10) tft.print(' ');
    tft.print(day);


    if (hasEvent) tft.fillCircle(x + 16, y + 6, 1, COLOR_GREEN);
  }


  tft.fillRect(0, 107, 160, 21, COLOR_BG);
  tft.drawFastHLine(0, 107, 160, COLOR_GRAY);
  uiTextSmall(2, 116, T("4/6 ДЕН 2/8 НЕД B/C МЕС", "4/6 DAY 2/8 WEEK B/C MON"), COLOR_GRAY);
  uiTextSmall(2, 125, T("A СОБЫТИЕ # СЕГ D НАЗ", "A EVENT # TODAY D BACK"), COLOR_GRAY);
}

void calendarKey(char key) {
  if (key == '4') { calendarMoveDays(-1); drawCalendar(); return; }
  if (key == '6') { calendarMoveDays(1); drawCalendar(); return; }
  if (key == '2') { calendarMoveDays(-7); drawCalendar(); return; }
  if (key == '8') { calendarMoveDays(7); drawCalendar(); return; }
  if (key == 'B') { calendarMoveMonth(-1); drawCalendar(); return; }
  if (key == 'C') { calendarMoveMonth(1); drawCalendar(); return; }
  if (key == '#') { initCalendarToToday(); drawCalendar(); return; }
  if (key == 'A') { openCalendarEventEditor(); return; }
  if (key == 'D') {
    currentScreen = SCREEN_DATETIME_MENU;
    drawDateTimeMenu();
  }
}

String calendarEventModeName() {
  if (calendarEventMode == INPUT_LAT_LOWER) return "abc";
  if (calendarEventMode == INPUT_LAT_UPPER) return "ABC";
  if (calendarEventMode == INPUT_RUS_LOWER) return systemLanguage == LANG_RU ? "рус" : "rus";
  if (calendarEventMode == INPUT_RUS_UPPER) return systemLanguage == LANG_RU ? "РУС" : "RUS";
  if (calendarEventMode == INPUT_NUMBERS) return "123";
  if (calendarEventMode == INPUT_NAV) return systemLanguage == LANG_RU ? "НАВ" : "NAV";
  return "abc";
}

void openCalendarEventEditor() {
  calendarEventText = loadCalendarEvent(calendarYear, calendarMonth, calendarSelectedDay);
  calendarEventCursor = calendarEventText.length();
  calendarEventMode = systemLanguage == LANG_RU ? INPUT_RUS_LOWER : INPUT_LAT_LOWER;
  calendarEventMultiActive = false;
  calendarEventDirty = false;
  currentScreen = SCREEN_CALENDAR_EVENT;
  drawCalendarEventEditor();
}

void drawCalendarEventText() {
  const int textWidth = 100;
  tft.fillRect(0, 34, textWidth, 73, COLOR_BG);
  const int cols = 16, rows = 5, x0 = 3, y0 = 47, lineH = 12;
  if (calendarEventCursor < 0) calendarEventCursor = 0;
  if (calendarEventCursor > calendarEventText.length()) calendarEventCursor = calendarEventText.length();
  int cursorGlyph = sharedCursorGlyphIndex(calendarEventText, calendarEventCursor);
  int firstGlyph = 0;
  if (cursorGlyph >= cols * rows) firstGlyph = (cursorGlyph / cols - rows + 1) * cols;
  int byteIndex = sharedCursorByteForGlyph(calendarEventText, firstGlyph);
  int glyphNo = firstGlyph, line = 0, col = 0;
  while (byteIndex < calendarEventText.length() && line < rows) {
    if (glyphNo == cursorGlyph) tft.drawFastVLine(x0 + col*6, y0 + line*lineH - 10, 10, COLOR_ACCENT);
    String g = utf8GlyphAt(calendarEventText, byteIndex);
    int next = utf8NextIndex(calendarEventText, byteIndex);
    if (g == "\n") { line++; col = 0; byteIndex = next; glyphNo++; continue; }
    drawUtf8(x0 + col * 6, y0 + line * lineH, g, COLOR_TEXT, u8g2_font_6x12_t_cyrillic);
    col++; glyphNo++; byteIndex = next;
    if (col >= cols) { line++; col = 0; }
  }
  if (cursorGlyph >= glyphNo && line < rows) tft.drawFastVLine(x0 + col*6, y0 + line*lineH - 10, 10, COLOR_ACCENT);
}

void drawCalendarEventHelp() {
  const int helpX = 103;
  tft.fillRect(helpX, 18, 57, 89, COLOR_BG);
  tft.drawFastVLine(helpX - 2, 18, 89, COLOR_GRAY);

  int x = helpX + 2;
  int y = 24;
  auto line = [&](const String& text) {
    drawUtf8(x, y, text, COLOR_YELLOW, u8g2_font_5x7_t_cyrillic);
    y += 7;
  };

  if (calendarEventMode == INPUT_RUS_LOWER || calendarEventMode == INPUT_RUS_UPPER) {
    bool upper = (calendarEventMode == INPUT_RUS_UPPER);
    line("1 .,!?" );
    line(upper ? "2 АБВГ"  : "2 абвг");
    line(upper ? "3 ДЕЁЖЗ" : "3 деёжз");
    line(upper ? "4 ИЙКЛ"   : "4 ийкл");
    line(upper ? "5 МНОП"   : "5 мноп");
    line(upper ? "6 РСТУ"   : "6 рсту");
    line(upper ? "7 ФХЦЧ"   : "7 фхцч");
    line(upper ? "8 ШЩЪЫ"   : "8 шщъы");
    line(upper ? "9 ЬЭЮЯ"   : "9 ьэюя");
    line(T("0 пробел", "0 space"));
    line("* @ . _ -");
    line(T("# фикс", "# commit"));
  } else if (calendarEventMode == INPUT_NAV) {
    line(T("НАВИГАЦИЯ", "NAVIGATION")); line(T("4 <  6 >", "4 <  6 >"));
    line(T("2 ^  8 v", "2 ^  8 v")); line(T("A режим", "A mode"));
    line(T("B удалить", "B delete")); line(T("C сохранить", "C save"));
    line(T("D назад", "D back"));
  } else if (calendarEventMode == INPUT_NUMBERS) {
    line(T("ЦИФРЫ 123", "NUMBERS 123"));
    line(T("0..9 цифры", "0..9 digits"));
    line(T("A режим", "A mode"));
    line(T("B удалить", "B delete"));
    line(T("C сохранить", "C save"));
    line(T("D назад", "D back"));
  } else {
    bool upper = (calendarEventMode == INPUT_LAT_UPPER);
    line("1 .,!?" );
    line(upper ? "2 ABC"  : "2 abc");
    line(upper ? "3 DEF"  : "3 def");
    line(upper ? "4 GHI"  : "4 ghi");
    line(upper ? "5 JKL"  : "5 jkl");
    line(upper ? "6 MNO"  : "6 mno");
    line(upper ? "7 PQRS" : "7 pqrs");
    line(upper ? "8 TUV"  : "8 tuv");
    line(upper ? "9 WXYZ" : "9 wxyz");
    line(T("0 пробел", "0 space"));
    line("* @ . _ -");
    line(T("# фикс", "# commit"));
  }
}

void drawCalendarEventEditor() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(4, 13, T("СОБЫТИЕ", "EVENT"), COLOR_TEXT);
  uiTextSmall(132, 13, calendarEventModeName(), COLOR_YELLOW);

  char dateBuf[16];
  snprintf(dateBuf, sizeof(dateBuf), "%02d.%02d.%04d", calendarSelectedDay, calendarMonth, calendarYear);
  drawUtf8(3, 30, String(dateBuf), COLOR_ACCENT, u8g2_font_5x7_t_cyrillic);

  drawCalendarEventText();
  drawCalendarEventHelp();

  tft.fillRect(0, 107, 160, 21, COLOR_BG);
  tft.drawFastHLine(0, 107, 160, COLOR_GRAY);
  uiTextSmall(2, 116, T("A РЕЖ B УДАЛ C СОХР", "A MODE B DEL C SAVE"), COLOR_GRAY);
  uiTextSmall(2, 125, T("D НАЗАД", "D BACK"), calendarEventDirty ? COLOR_YELLOW : COLOR_GRAY);
}

void calendarEventMultiTap(char key) {
  if (calendarEventMode == INPUT_NAV) return;
  if (calendarEventMode == INPUT_NUMBERS && key >= '0' && key <= '9') {
    sharedInsert(calendarEventText, calendarEventCursor, String(key), MAX_CALENDAR_EVENT_BYTES);
    calendarEventMultiActive = false; calendarEventDirty = true; drawCalendarEventText(); return;
  }
  String set = getCharacterSet(key, calendarEventMode);
  if (!set.length()) return;
  unsigned long now = millis();
  bool sameKey = calendarEventMultiActive && key == calendarEventLastKey && now - calendarEventLastTime < 900;
  if (sameKey && calendarEventCursor > 0) {
    int glyphCount = utf8Count(set); calendarEventMultiIndex = (calendarEventMultiIndex + 1) % max(1, glyphCount);
    utf8ReplacePrevious(calendarEventText, calendarEventCursor, utf8GlyphAtNumber(set, calendarEventMultiIndex));
  } else {
    calendarEventMultiIndex = 0; sharedInsert(calendarEventText, calendarEventCursor, utf8GlyphAtNumber(set, 0), MAX_CALENDAR_EVENT_BYTES);
  }
  calendarEventLastKey = key; calendarEventLastTime = now; calendarEventMultiActive = true;
  calendarEventDirty = true; drawCalendarEventText();
}

void calendarEventKey(char key) {
  if (key == 'A') {
    calendarEventMultiActive = false;
    if (calendarEventMode == INPUT_LAT_LOWER) calendarEventMode = INPUT_LAT_UPPER;
    else if (calendarEventMode == INPUT_LAT_UPPER) calendarEventMode = INPUT_RUS_LOWER;
    else if (calendarEventMode == INPUT_RUS_LOWER) calendarEventMode = INPUT_RUS_UPPER;
    else if (calendarEventMode == INPUT_RUS_UPPER) calendarEventMode = INPUT_NUMBERS;
    else if (calendarEventMode == INPUT_NUMBERS) calendarEventMode = INPUT_NAV;
    else calendarEventMode = INPUT_LAT_LOWER;
    drawCalendarEventEditor();
    return;
  }

  if (calendarEventMode == INPUT_NAV && (key == '2' || key == '8' || key == '4' || key == '6')) {
    calendarEventMultiActive = false;
    if (key == '4') sharedCursorMoveHorizontal(calendarEventText, calendarEventCursor, -1);
    else if (key == '6') sharedCursorMoveHorizontal(calendarEventText, calendarEventCursor, 1);
    else if (key == '2') sharedCursorMoveVertical(calendarEventText, calendarEventCursor, 16, -1);
    else if (key == '8') sharedCursorMoveVertical(calendarEventText, calendarEventCursor, 16, 1);
    drawCalendarEventText(); return;
  }

  if (key == 'B') {
    calendarEventMultiActive = false;
    if (sharedBackspace(calendarEventText, calendarEventCursor)) { calendarEventDirty = true; drawCalendarEventEditor(); }
    return;
  }

  if (key == 'C') {
    calendarEventMultiActive = false;
    bool ok = saveCalendarEvent(calendarYear, calendarMonth, calendarSelectedDay, calendarEventText);
    if (ok) {
      calendarEventDirty = false;
      showMessage(calendarEventText.length() ? T("СОБЫТИЕ СОХРАНЕНО", "EVENT SAVED") : T("СОБЫТИЕ УДАЛЕНО", "EVENT REMOVED"), "", COLOR_GREEN, 650);
      currentScreen = SCREEN_CALENDAR;
      drawCalendar();
    } else {
      showMessage(T("ОШИБКА СОХРАНЕНИЯ", "SAVE ERROR"), "", COLOR_RED, 800);
      currentScreen = SCREEN_CALENDAR_EVENT;
      drawCalendarEventEditor();
    }
    return;
  }

  if (key == 'D') {
    calendarEventMultiActive = false;
    currentScreen = SCREEN_CALENDAR;
    drawCalendar();
    return;
  }

  if (key == '#') {
    calendarEventMultiActive = false;
    return;
  }

  calendarEventMultiTap(key);
}




void drawWiFiMenu() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  tft.setTextSize(1); tft.setTextColor(COLOR_TEXT); tft.setCursor(5, 5); tft.print("WI-FI");
  uiText(118, 12, WiFi.status() == WL_CONNECTED ? T("В СЕТИ", "ONLINE") : T("НЕТ СЕТИ", "OFFLINE"), WiFi.status() == WL_CONNECTED ? COLOR_GREEN : COLOR_RED);
  for (int i = 0; i < WIFI_MENU_ITEMS; i++) {
    int y = 37 + i * 29; uint16_t c = COLOR_GRAY;
    if (i == wifiMenuSelected) { tft.fillRoundRect(5, y - 5, 150, 21, 3, COLOR_SELECT); c = COLOR_TEXT; }
    tft.setTextColor(c); tft.setCursor(9, y); tft.print(i + 1); tft.print(".");
    uiText(23, y + 7, wifiMenuItemText(i), c);
  }
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(4, 123, T("2/8 ВЫБОР A ОТКР D НАЗАД", "2/8 MOVE A OPEN D BACK"), COLOR_GRAY);
}

void wifiMenuKey(char key) {
  if (key == '2') {
    wifiMenuSelected--;
    if (wifiMenuSelected < 0) wifiMenuSelected = WIFI_MENU_ITEMS - 1;
    drawWiFiMenu(); return;
  }
  if (key == '8') {
    wifiMenuSelected++;
    if (wifiMenuSelected >= WIFI_MENU_ITEMS) wifiMenuSelected = 0;
    drawWiFiMenu(); return;
  }
  if (key == 'A' || key == '#') {
    if (wifiMenuSelected == 0) {
      currentScreen = SCREEN_WIFI;
      scanWiFiNetworks();
    } else {
      currentScreen = SCREEN_HTTP_SHARE;
      if (WiFi.status() == WL_CONNECTED && !httpServerRunning) startHttpServer();
      drawHttpShare();
    }
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_MENU;
    drawMenu();
  }
}

void connectSavedWiFi() {
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  if (!ssid.length()) return;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) delay(100);
  if (WiFi.status() == WL_CONNECTED) configureKaliningradTime();
}

void scanWiFiNetworks() {
  currentScreen = SCREEN_WIFI;
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("ПОИСК WI-FI", "WI-FI SCAN"), COLOR_TEXT);
  uiText(53, 62, T("Сканирование...", "Scanning..."), COLOR_YELLOW);
  WiFi.mode(WIFI_STA); WiFi.scanDelete();
  wifiNetworkCount = WiFi.scanNetworks(); wifiSelected = 0; wifiTop = 0;
  drawWiFiScreen();
}

void drawWiFiScreen() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  tft.setTextSize(1); tft.setTextColor(COLOR_TEXT); tft.setCursor(5, 5); tft.print("WI-FI");
  if (WiFi.status() == WL_CONNECTED) {
    uiText(5, 28, T("ПОДКЛЮЧЕНО:", "CONNECTED:"), COLOR_GREEN);
    String ssid = WiFi.SSID(); if (ssid.length() > 18) ssid = ssid.substring(0, 18);
    tft.setTextColor(COLOR_GREEN); tft.setCursor(70, 21); tft.print(ssid);
  } else uiText(5, 28, T("НЕТ ПОДКЛЮЧЕНИЯ", "NOT CONNECTED"), COLOR_RED);
  drawWiFiRows();
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(2, 123, T("2/8 ВЫБ A ОК C ПОИСК D НАЗ", "2/8 MOVE A OK C SCAN D BACK"), COLOR_GRAY);
}

void drawWiFiRows() {
  tft.fillRect(0, 31, 160, 75, COLOR_BG);
  if (wifiNetworkCount <= 0) {
    uiText(38, 62, T("СЕТИ НЕ НАЙДЕНЫ", "NO NETWORKS"), COLOR_YELLOW);
    return;
  }
  for (int row = 0; row < WIFI_VISIBLE_ROWS; row++) {
    int index = wifiTop + row; if (index >= wifiNetworkCount) break;
    int y = 33 + row * 18;
    if (index == wifiSelected) {
      tft.fillRoundRect(2, y - 2, 156, 16, 3, COLOR_SELECT);
      tft.setTextColor(COLOR_TEXT);
    } else tft.setTextColor(COLOR_GRAY);
    String ssid = WiFi.SSID(index);
    if (!ssid.length()) ssid = "<hidden>";
    if (ssid.length() > 17) ssid = ssid.substring(0, 17);
    tft.setCursor(5, y + 2); tft.print(ssid);
    int auth = WiFi.encryptionType(index);
    tft.setCursor(112, y + 2); tft.print(auth == WIFI_AUTH_OPEN ? "O" : "L");
    tft.setCursor(128, y + 2); tft.print(WiFi.RSSI(index));
  }
}

void wifiKey(char key) {
  if (key == '2' && wifiSelected > 0) {
    wifiSelected--; if (wifiSelected < wifiTop) wifiTop = wifiSelected;
    drawWiFiRows(); return;
  }
  if (key == '8' && wifiSelected < wifiNetworkCount - 1) {
    wifiSelected++;
    if (wifiSelected >= wifiTop + WIFI_VISIBLE_ROWS) wifiTop = wifiSelected - WIFI_VISIBLE_ROWS + 1;
    drawWiFiRows(); return;
  }
  if (key == 'A' || key == '#') { selectWiFiNetwork(); return; }
  if (key == 'C') { scanWiFiNetworks(); return; }
  if (key == 'D') { currentScreen = SCREEN_WIFI_MENU; drawWiFiMenu(); }
}

void selectWiFiNetwork() {
  if (wifiNetworkCount <= 0) return;
  selectedSSID = WiFi.SSID(wifiSelected);
  selectedAuth = WiFi.encryptionType(wifiSelected);
  if (selectedAuth == WIFI_AUTH_OPEN) {
    connectToWiFi(selectedSSID, "");
    return;
  }
  wifiPassword = "";
  passwordCursor = 0;
  passwordMode = INPUT_LAT_LOWER;
  passwordMultiActive = false;
  currentScreen = SCREEN_WIFI_PASSWORD;
  drawPasswordScreen();
}

String inputModeName(InputMode mode) {
  if (mode == INPUT_LAT_LOWER) return systemLanguage == LANG_RU ? "лат" : "abc";
  if (mode == INPUT_LAT_UPPER) return systemLanguage == LANG_RU ? "ЛАТ" : "ABC";
  if (mode == INPUT_RUS_LOWER) return systemLanguage == LANG_RU ? "рус" : "rus";
  if (mode == INPUT_RUS_UPPER) return systemLanguage == LANG_RU ? "РУС" : "RUS";
  if (mode == INPUT_NUMBERS) return "123";
  return systemLanguage == LANG_RU ? "НАВ" : "NAV";
}

String getCharacterSet(char key, InputMode mode) {



  bool programEditorSymbols =
    currentScreen == SCREEN_EDITOR && editorReturnScreen == SCREEN_PROGRAM_MENU;


  if (mode == INPUT_RUS_LOWER || mode == INPUT_RUS_UPPER) {
    bool upper = (mode == INPUT_RUS_UPPER);
    if (key == '1') return ".,!?1";
    if (key == '2') return upper ? "АБВГ2" : "абвг2";
    if (key == '3') return upper ? "ДЕЁЖЗ3" : "деёжз3";
    if (key == '4') return upper ? "ИЙКЛ4" : "ийкл4";
    if (key == '5') return upper ? "МНОП5" : "мноп5";
    if (key == '6') return upper ? "РСТУ6" : "рсту6";
    if (key == '7') return upper ? "ФХЦЧ7" : "фхцч7";
    if (key == '8') return upper ? "ШЩЪЫ8" : "шщъы8";
    if (key == '9') return upper ? "ЬЭЮЯ9" : "ьэюя9";
    if (key == '0') return " 0";
    if (key == '*') return programEditorSymbols ? "@._-!$%&?+=<>/()" : "@._-!$%&?+";
    return "";
  }

  String set = "";
  if (key == '1') set = ".,!?1";
  else if (key == '2') set = "abc2";
  else if (key == '3') set = "def3";
  else if (key == '4') set = "ghi4";
  else if (key == '5') set = "jkl5";
  else if (key == '6') set = "mno6";
  else if (key == '7') set = "pqrs7";
  else if (key == '8') set = "tuv8";
  else if (key == '9') set = "wxyz9";
  else if (key == '0') set = " 0";
  else if (key == '*') set = programEditorSymbols ? "@._-!$%&?+=<>/()" : "@._-!$%&?+";

  if (mode == INPUT_LAT_UPPER) {
    for (int i = 0; i < set.length(); i++) {
      if (isalpha((unsigned char)set[i])) set.setCharAt(i, toupper((unsigned char)set[i]));
    }
  }
  return set;
}

void drawPasswordScreen() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("ПАРОЛЬ WI-FI", "WI-FI PASSWORD"), COLOR_TEXT);
  tft.setTextColor(COLOR_ACCENT); tft.setCursor(5, 24);
  String ssid = selectedSSID; if (ssid.length() > 24) ssid = ssid.substring(0, 24); tft.print(ssid);
  tft.drawRoundRect(4, 37, 152, 23, 3, COLOR_GRAY); updatePasswordField();
  uiText(5, 72, T("РЕЖИМ:", "MODE:"), COLOR_YELLOW); uiText(45, 72, inputModeName(passwordMode), COLOR_YELLOW);
  if (passwordMode == INPUT_NAV) {
    uiTextSmall(5, 87, T("4 <  6 >   2 НАЧ 8 КОН", "4 <  6 >   2 HOME 8 END"), COLOR_GRAY);
    uiTextSmall(5, 99, T("A = СМЕНА РЕЖИМА", "A = CHANGE MODE"), COLOR_GRAY);
  } else {
    tft.setTextColor(COLOR_GRAY); tft.setCursor(5, 79); tft.print("2 ABC 3 DEF 4 GHI");
    tft.setCursor(5, 89); tft.print("5 JKL 6 MNO 7 PQRS");
    tft.setCursor(5, 99); tft.print("8 TUV 9 WXYZ  * @");
  }
  tft.drawFastHLine(0, 109, 160, COLOR_GRAY);
  uiTextSmall(1, 123, T("A РЕЖ B УДАЛ C ВХОД D НАЗ", "A MODE B DEL C JOIN D BACK"), COLOR_GRAY);
}

void updatePasswordField() {
  tft.fillRect(7, 40, 146, 17, COLOR_BG);
  if (passwordCursor < 0) passwordCursor = 0;
  if (passwordCursor > wifiPassword.length()) passwordCursor = wifiPassword.length();
  int startByte = passwordCursor > 20 ? passwordCursor - 20 : 0;
  if (startByte > wifiPassword.length()) startByte = wifiPassword.length();
  String shown = wifiPassword.substring(startByte);
  if (shown.length() > 22) shown = shown.substring(0, 22);
  tft.setTextSize(1); tft.setTextColor(COLOR_GREEN, COLOR_BG);
  tft.setCursor(10, 47); tft.print(shown);
  int cx = 10 + (passwordCursor - startByte) * 6;
  if (cx < 10) cx = 10; if (cx > 150) cx = 150;
  tft.drawFastVLine(cx, 42, 11, COLOR_ACCENT);
}

void passwordMultiTap(char key) {
  if (passwordMode == INPUT_NAV) return;
  if (passwordMode == INPUT_NUMBERS && key >= '0' && key <= '9') {
    if (wifiPassword.length() < 63) { wifiPassword = wifiPassword.substring(0, passwordCursor) + String(key) + wifiPassword.substring(passwordCursor); passwordCursor++; }
    passwordMultiActive = false; updatePasswordField(); return;
  }
  String set = getCharacterSet(key, passwordMode);
  if (!set.length()) return;
  unsigned long now = millis();
  bool sameKey = passwordMultiActive && key == passwordLastKey && now - passwordLastTime < 900;
  if (sameKey && passwordCursor > 0) {
    passwordMultiIndex = (passwordMultiIndex + 1) % max(1, (int)set.length());
    wifiPassword.setCharAt(passwordCursor - 1, set[passwordMultiIndex]);
  } else {
    if (wifiPassword.length() >= 63) return;
    passwordMultiIndex = 0;
    wifiPassword = wifiPassword.substring(0, passwordCursor) + String(set[0]) + wifiPassword.substring(passwordCursor);
    passwordCursor++;
  }
  passwordLastKey = key; passwordLastTime = now; passwordMultiActive = true; updatePasswordField();
}

void passwordKey(char key) {
  if (key == 'A') {
    passwordMultiActive = false;
    if (passwordMode == INPUT_LAT_LOWER) passwordMode = INPUT_LAT_UPPER;
    else if (passwordMode == INPUT_LAT_UPPER) passwordMode = INPUT_NUMBERS;
    else if (passwordMode == INPUT_NUMBERS) passwordMode = INPUT_NAV;
    else passwordMode = INPUT_LAT_LOWER;
    drawPasswordScreen(); return;
  }
  if (passwordMode == INPUT_NAV && (key == '2' || key == '8' || key == '4' || key == '6')) {
    passwordMultiActive = false;
    if (key == '4' && passwordCursor > 0) passwordCursor--;
    else if (key == '6' && passwordCursor < wifiPassword.length()) passwordCursor++;
    else if (key == '2') passwordCursor = 0;
    else if (key == '8') passwordCursor = wifiPassword.length();
    updatePasswordField(); return;
  }
  if (key == 'B') {
    passwordMultiActive = false;
    if (passwordCursor > 0 && wifiPassword.length()) { wifiPassword.remove(passwordCursor - 1, 1); passwordCursor--; }
    updatePasswordField(); return;
  }
  if (key == 'C') { passwordMultiActive = false; connectToWiFi(selectedSSID, wifiPassword); return; }
  if (key == 'D') { passwordMultiActive = false; currentScreen = SCREEN_WIFI; drawWiFiScreen(); return; }
  if (key == '#') { passwordMultiActive = false; return; }
  passwordMultiTap(key);
}

void connectToWiFi(String ssid, String password) {
  tft.fillScreen(COLOR_BG); tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("ПОДКЛЮЧЕНИЕ WI-FI", "WI-FI CONNECT"), COLOR_TEXT);
  tft.setTextColor(COLOR_ACCENT); tft.setCursor(10, 42); tft.print(ssid);
  uiText(48, 67, T("Подключение...", "Connecting..."), COLOR_YELLOW);
  WiFi.mode(WIFI_STA); WiFi.disconnect(); delay(200); WiFi.begin(ssid.c_str(), password.c_str());
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 12000) delay(250);
  if (WiFi.status() == WL_CONNECTED) {
    preferences.putString("ssid", ssid); preferences.putString("password", password);
    configureKaliningradTime(); showMessage(T("ПОДКЛЮЧЕНО", "CONNECTED"), WiFi.localIP().toString(), COLOR_GREEN, 1000);
    scanWiFiNetworks();
  } else {
    showMessage(T("ОШИБКА ПОДКЛЮЧЕНИЯ", "CONNECTION FAILED"), "", COLOR_RED, 1200);
    currentScreen = SCREEN_WIFI; drawWiFiScreen();
  }
}




int utf8SeqLen(uint8_t lead) {
  if ((lead & 0x80) == 0) return 1;
  if ((lead & 0xE0) == 0xC0) return 2;
  if ((lead & 0xF0) == 0xE0) return 3;
  if ((lead & 0xF8) == 0xF0) return 4;
  return 1;
}

int utf8NextIndex(const String& s, int byteIndex) {
  if (byteIndex < 0) return 0;
  if (byteIndex >= s.length()) return s.length();
  int n = utf8SeqLen((uint8_t)s[byteIndex]);
  int out = byteIndex + n;
  if (out > s.length()) out = s.length();
  return out;
}

int utf8PrevIndex(const String& s, int byteIndex) {
  if (byteIndex <= 0) return 0;
  int i = byteIndex - 1;
  while (i > 0 && (((uint8_t)s[i] & 0xC0) == 0x80)) i--;
  return i;
}

String utf8GlyphAt(const String& s, int byteIndex) {
  if (byteIndex < 0 || byteIndex >= s.length()) return "";
  int next = utf8NextIndex(s, byteIndex);
  return s.substring(byteIndex, next);
}

String utf8GlyphAtNumber(const String& s, int glyphNumber) {
  int i = 0, n = 0;
  while (i < s.length()) {
    if (n == glyphNumber) return utf8GlyphAt(s, i);
    i = utf8NextIndex(s, i);
    n++;
  }
  return "";
}

int utf8Count(const String& s) {
  int i = 0, count = 0;
  while (i < s.length()) { i = utf8NextIndex(s, i); count++; }
  return count;
}

String utf8Truncate(const String& s, int maxGlyphs) {
  int i = 0, count = 0;
  while (i < s.length() && count < maxGlyphs) { i = utf8NextIndex(s, i); count++; }
  return s.substring(0, i);
}

void utf8InsertAt(String& s, int byteIndex, const String& glyph) {
  s = s.substring(0, byteIndex) + glyph + s.substring(byteIndex);
}

void utf8ReplacePrevious(String& s, int& cursor, const String& glyph) {
  if (cursor <= 0) return;
  int prev = utf8PrevIndex(s, cursor);
  s = s.substring(0, prev) + glyph + s.substring(cursor);
  cursor = prev + glyph.length();
}


void sharedCursorMoveHorizontal(const String& text, int& cursor, int delta) {
  if (cursor < 0) cursor = 0;
  if (cursor > text.length()) cursor = text.length();
  if (delta < 0) cursor = utf8PrevIndex(text, cursor);
  else if (delta > 0) cursor = utf8NextIndex(text, cursor);
}

int sharedCursorGlyphIndex(const String& text, int cursor) {
  int i = 0, n = 0;
  while (i < cursor && i < text.length()) { i = utf8NextIndex(text, i); n++; }
  return n;
}

int sharedCursorByteForGlyph(const String& text, int glyphIndex) {
  if (glyphIndex <= 0) return 0;
  int i = 0, n = 0;
  while (i < text.length() && n < glyphIndex) { i = utf8NextIndex(text, i); n++; }
  return i;
}

void sharedCursorMoveVertical(const String& text, int& cursor, int columns, int deltaRows) {
  int glyph = sharedCursorGlyphIndex(text, cursor);
  int target = glyph + deltaRows * columns;
  int total = utf8Count(text);
  if (target < 0) target = 0;
  if (target > total) target = total;
  cursor = sharedCursorByteForGlyph(text, target);
}

bool sharedBackspace(String& text, int& cursor) {
  if (cursor <= 0 || !text.length()) return false;
  int prev = utf8PrevIndex(text, cursor);
  text.remove(prev, cursor - prev);
  cursor = prev;
  return true;
}

bool sharedInsert(String& text, int& cursor, const String& glyph, int maxBytes) {
  if (text.length() + glyph.length() > maxBytes) return false;
  utf8InsertAt(text, cursor, glyph);
  cursor += glyph.length();
  return true;
}





void drawRecordsMenu() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ЗАПИСИ", "NOTES"), COLOR_TEXT);

  const char* ru[RECORDS_MENU_ITEMS] = {
    "ОБЫЧНЫЕ ЗАМЕТКИ",
    "ЗАШИФРОВАННЫЕ",
    "АДРЕСНАЯ КНИГА"
  };
  const char* en[RECORDS_MENU_ITEMS] = {
    "PLAIN NOTES",
    "ENCRYPTED NOTES",
    "ADDRESS BOOK"
  };

  for (int i = 0; i < RECORDS_MENU_ITEMS; i++) {
    int y = 31 + i * 23;
    uint16_t c = COLOR_GRAY;
    if (i == recordsMenuSelected) {
      tft.fillRoundRect(4, y - 14, 152, 20, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    uiText(9, y, systemLanguage == LANG_RU ? String(ru[i]) : String(en[i]), c);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(7, 123, T("2/8 ВЫБ  A/# ОТКР  D НАЗ", "2/8 SEL  A/# OPEN  D BACK"), COLOR_GRAY);
}

void recordsMenuKey(char key) {
  if (key == '2') {
    recordsMenuSelected--;
    if (recordsMenuSelected < 0) recordsMenuSelected = RECORDS_MENU_ITEMS - 1;
    drawRecordsMenu();
    return;
  }
  if (key == '8') {
    recordsMenuSelected++;
    if (recordsMenuSelected >= RECORDS_MENU_ITEMS) recordsMenuSelected = 0;
    drawRecordsMenu();
    return;
  }
  if (key == 'A' || key == '#') {
    if (recordsMenuSelected == 0) {
      currentScreen = SCREEN_NOTES;
      refreshNoteList();
      drawNotesScreen();
    } else if (recordsMenuSelected == 1) {
      beginSecureVault(VAULT_TARGET_NOTES);
    } else {
      beginSecureVault(VAULT_TARGET_CONTACTS);
    }
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_MENU;
    drawMenu();
  }
}




String contactStripLineEnd(String v) {
  while (v.endsWith("\r") || v.endsWith("\n")) v.remove(v.length() - 1);
  return v;
}

String serializeContact(const String& name, const String& phone, const String& email, const String& address) {


  String n = name, p = phone, e = email, a = address;
  n.replace("\r", " "); n.replace("\n", " ");
  p.replace("\r", " "); p.replace("\n", " ");
  e.replace("\r", " "); e.replace("\n", " ");
  a.replace("\r", " "); a.replace("\n", " ");
  return String("PPC_CONTACT_V2\n") + n + "\n" + p + "\n" + e + "\n" + a;
}

static bool contactReadPayloadLine(const String& plain, int& pos, String& out) {
  if (pos < 0 || pos > plain.length()) return false;
  int nl = plain.indexOf('\n', pos);
  if (nl < 0) {
    out = plain.substring(pos);
    pos = plain.length();
  } else {
    out = plain.substring(pos, nl);
    pos = nl + 1;
  }
  out = contactStripLineEnd(out);
  return true;
}

bool parseContactPayload(const String& plain, String& name, String& phone, String& email, String& address) {
  name = ""; phone = ""; email = ""; address = "";
  int pos = 0;
  String magic;
  if (!contactReadPayloadLine(plain, pos, magic) || magic != "PPC_CONTACT_V2") return false;
  if (!contactReadPayloadLine(plain, pos, name)) return false;
  if (!contactReadPayloadLine(plain, pos, phone)) return false;
  if (!contactReadPayloadLine(plain, pos, email)) return false;
  if (pos <= plain.length()) address = plain.substring(pos);
  address = contactStripLineEnd(address);
  return true;
}

bool contactFileHasSecureHeader(const String& path) {
  File f = LittleFS.open(path, "r");
  if (!f || f.isDirectory()) { if (f) f.close(); return false; }
  uint8_t header[8] = {0};
  bool ok = f.read(header, sizeof(header)) == sizeof(header);
  f.close();
  const uint8_t expected[8] = {'P','P','C','S','E','C','0','1'};
  return ok && memcmp(header, expected, sizeof(expected)) == 0;
}

bool loadLegacyPlainContact(const String& path, String& name, String& phone, String& email, String& address) {
  name = ""; phone = ""; email = ""; address = "";
  File f = LittleFS.open(path, "r");
  if (!f || f.isDirectory()) { if (f) f.close(); return false; }

  String magic = contactStripLineEnd(f.readStringUntil('\n'));
  if (magic == "PPC_CONTACT_V1") {
    name = contactStripLineEnd(f.readStringUntil('\n'));
    phone = contactStripLineEnd(f.readStringUntil('\n'));
    email = contactStripLineEnd(f.readStringUntil('\n'));
    address = contactStripLineEnd(f.readStringUntil('\n'));
  } else {

    name = magic;
    phone = contactStripLineEnd(f.readStringUntil('\n'));
    email = contactStripLineEnd(f.readStringUntil('\n'));
    address = contactStripLineEnd(f.readStringUntil('\n'));
  }
  f.close();
  return name.length() || phone.length() || email.length() || address.length();
}

bool loadContact(const String& path, String& name, String& phone, String& email, String& address) {
  name = ""; phone = ""; email = ""; address = "";
  if (!secureVaultUnlocked || !secureSessionPin.length()) return false;


  String plain;
  if (secureDecryptFile(path, plain, secureSessionPin)) {
    return parseContactPayload(plain, name, phone, email, address);
  }



  if (contactFileHasSecureHeader(path)) return false;
  if (!loadLegacyPlainContact(path, name, phone, email, address)) return false;

  String migrated = serializeContact(name, phone, email, address);
  if (!secureEncryptFile(path, migrated, secureSessionPin)) {
    name = ""; phone = ""; email = ""; address = "";
    return false;
  }
  return true;
}

bool saveContact(const String& path, const String& name, const String& phone, const String& email, const String& address) {
  if (!secureVaultUnlocked || !secureSessionPin.length()) return false;
  if (!LittleFS.exists("/contacts")) LittleFS.mkdir("/contacts");
  return secureEncryptFile(path, serializeContact(name, phone, email, address), secureSessionPin);
}

String createContactPath() {
  if (!LittleFS.exists("/contacts")) LittleFS.mkdir("/contacts");
  char path[32];
  for (int i = 1; i <= 99; i++) {
    snprintf(path, sizeof(path), "/contacts/C%02d.CTC", i);
    if (!LittleFS.exists(path)) return String(path);
  }
  return "";
}

void refreshContactList() {
  contactCount = 0;
  if (!LittleFS.exists("/contacts")) LittleFS.mkdir("/contacts");
  char path[32];
  for (int i = 1; i <= 99 && contactCount < MAX_CONTACTS; i++) {
    snprintf(path, sizeof(path), "/contacts/C%02d.CTC", i);
    if (!LittleFS.exists(path)) continue;
    String n, p, e, a;
    if (!loadContact(String(path), n, p, e, a)) continue;
    contactPaths[contactCount] = String(path);
    contactNames[contactCount] = n.length() ? n : T("БЕЗ ИМЕНИ", "NO NAME");
    contactCount++;
  }
  if (contactSelected >= contactCount) contactSelected = contactCount - 1;
  if (contactSelected < 0) contactSelected = 0;
  if (contactTop > contactSelected) contactTop = contactSelected;
  if (contactSelected >= contactTop + CONTACT_VISIBLE_ROWS) contactTop = contactSelected - CONTACT_VISIBLE_ROWS + 1;
  if (contactTop < 0) contactTop = 0;
  contactDeleteArmed = false;
}

void drawContactRows() {
  tft.fillRect(0, 19, 160, 88, COLOR_BG);
  if (contactCount == 0) {
    drawUiCentered(T("НЕТ КОНТАКТОВ", "NO CONTACTS"), 58, COLOR_GRAY, u8g2_font_6x12_t_cyrillic, 6);
    drawUiCentered(T("B - НОВЫЙ", "B - NEW"), 76, COLOR_YELLOW, u8g2_font_5x7_t_cyrillic, 5);
    return;
  }

  for (int row = 0; row < CONTACT_VISIBLE_ROWS; row++) {
    int i = contactTop + row;
    if (i >= contactCount) break;
    int y = 24 + row * 21;
    uint16_t c = COLOR_GRAY;
    if (i == contactSelected) {
      tft.fillRoundRect(3, y - 4, 154, 19, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    String shown = utf8Truncate(contactNames[i], 23);
    drawUtf8(8, y + 8, shown, c, u8g2_font_6x12_t_cyrillic);
  }
}

void drawContactsScreen() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(4, 13, T("АДРЕСНАЯ КНИГА", "ADDRESS BOOK"), COLOR_TEXT);
  tft.setTextSize(1); tft.setTextColor(COLOR_TEXT); tft.setCursor(145, 5); tft.print(contactCount);
  drawContactRows();
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  if (contactDeleteArmed) {
    uiTextSmall(3, 122, T("C ЕЩЁ РАЗ = УДАЛИТЬ", "C AGAIN = DELETE"), COLOR_RED);
  } else {
    uiTextSmall(2, 116, T("2/8 ВЫБ A ОТКР B НОВ", "2/8 SEL A OPEN B NEW"), COLOR_GRAY);
    uiTextSmall(2, 125, T("C C УДАЛ  D НАЗ", "C C DEL   D BACK"), COLOR_GRAY);
  }
}

void contactsKey(char key) {
  if (key != 'C') contactDeleteArmed = false;
  if (key == '2' && contactCount > 0) {
    contactSelected--;
    if (contactSelected < 0) contactSelected = contactCount - 1;
    if (contactSelected < contactTop) contactTop = contactSelected;
    if (contactSelected >= contactTop + CONTACT_VISIBLE_ROWS) contactTop = contactSelected - CONTACT_VISIBLE_ROWS + 1;
    drawContactsScreen(); return;
  }
  if (key == '8' && contactCount > 0) {
    contactSelected++;
    if (contactSelected >= contactCount) contactSelected = 0;
    if (contactSelected < contactTop) contactTop = contactSelected;
    if (contactSelected >= contactTop + CONTACT_VISIBLE_ROWS) contactTop = contactSelected - CONTACT_VISIBLE_ROWS + 1;
    drawContactsScreen(); return;
  }
  if (key == 'A' || key == '#') {
    if (contactCount > 0) openSelectedContact();
    return;
  }
  if (key == 'B') { startNewContact(); return; }
  if (key == 'C') {
    if (contactCount <= 0) return;
    if (!contactDeleteArmed) { contactDeleteArmed = true; drawContactsScreen(); return; }
    deleteSelectedContact(); return;
  }
  if (key == 'D') {
    closeSecureVault();
    currentScreen = SCREEN_RECORDS_MENU;
    drawRecordsMenu();
  }
}

void openSelectedContact() {
  if (contactCount <= 0 || contactSelected < 0 || contactSelected >= contactCount) return;
  contactEditPath = contactPaths[contactSelected];
  if (!loadContact(contactEditPath, contactName, contactPhone, contactEmail, contactAddress)) {
    showMessage(T("ОШИБКА КОНТАКТА", "CONTACT ERROR"), "", COLOR_RED, 700);
    currentScreen = SCREEN_CONTACTS; drawContactsScreen(); return;
  }
  contactDeleteArmed = false;
  currentScreen = SCREEN_CONTACT_VIEW;
  drawContactView();
}

void drawContactWrapped(const String& value, int x, int baseline, int cols, int maxLines, uint16_t color) {

  int i = 0, col = 0, line = 0;
  while (i < value.length() && line < maxLines) {
    String g = utf8GlyphAt(value, i);
    if (col >= cols) { line++; col = 0; if (line >= maxLines) break; }
    drawUtf8(x + col * 6, baseline + line * 12, g, color, u8g2_font_6x12_t_cyrillic);
    col++;
    i = utf8NextIndex(value, i);
  }
}

void drawContactView() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(4, 13, T("КОНТАКТ", "CONTACT"), COLOR_TEXT);

  drawUtf8(4, 31, utf8Truncate(contactName.length() ? contactName : T("БЕЗ ИМЕНИ", "NO NAME"), 25), COLOR_ACCENT, u8g2_font_6x12_t_cyrillic);

  uiTextSmall(4, 45, T("ТЕЛ:", "TEL:"), COLOR_GRAY);
  drawContactWrapped(contactPhone.length() ? contactPhone : "-", 30, 46, 21, 1, COLOR_TEXT);
  uiTextSmall(4, 62, "E-MAIL:", COLOR_GRAY);
  drawContactWrapped(contactEmail.length() ? contactEmail : "-", 40, 63, 19, 1, COLOR_TEXT);
  uiTextSmall(4, 78, T("ИНФО:", "INFO:"), COLOR_GRAY);
  drawContactWrapped(contactAddress.length() ? contactAddress : "-", 4, 90, 25, 2, COLOR_TEXT);

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  if (contactDeleteArmed) {
    uiTextSmall(3, 122, T("C ЕЩЁ РАЗ = УДАЛИТЬ", "C AGAIN = DELETE"), COLOR_RED);
  } else {
    uiTextSmall(2, 116, T("A РЕДАКТ  C C УДАЛ", "A EDIT   C C DELETE"), COLOR_GRAY);
    uiTextSmall(2, 125, T("D/B НАЗАД", "D/B BACK"), COLOR_GRAY);
  }
}

void contactViewKey(char key) {
  if (key != 'C') contactDeleteArmed = false;
  if (key == 'A' || key == '#') { startEditContact(); return; }
  if (key == 'C') {
    if (!contactDeleteArmed) { contactDeleteArmed = true; drawContactView(); return; }
    deleteSelectedContact(); return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_CONTACTS;
    refreshContactList();
    drawContactsScreen();
  }
}

void startNewContact() {
  contactEditPath = createContactPath();
  if (!contactEditPath.length()) {
    showMessage(T("ЛИМИТ КОНТАКТОВ", "CONTACT LIMIT"), "", COLOR_RED, 700);
    currentScreen = SCREEN_CONTACTS; drawContactsScreen(); return;
  }
  contactName = ""; contactPhone = ""; contactEmail = ""; contactAddress = "";
  contactEditField = 0;
  contactCursor = 0;
  contactInputMode = systemLanguage == LANG_RU ? INPUT_RUS_LOWER : INPUT_LAT_LOWER;
  contactMultiActive = false;
  contactEditingExisting = false;
  currentScreen = SCREEN_CONTACT_EDIT;
  drawContactEditor();
}

void startEditContact() {
  contactEditField = 0;
  contactCursor = contactName.length();
  contactInputMode = systemLanguage == LANG_RU ? INPUT_RUS_LOWER : INPUT_LAT_LOWER;
  contactMultiActive = false;
  contactEditingExisting = true;
  currentScreen = SCREEN_CONTACT_EDIT;
  drawContactEditor();
}

String* contactActiveField() {
  if (contactEditField == 0) return &contactName;
  if (contactEditField == 1) return &contactPhone;
  if (contactEditField == 2) return &contactEmail;
  return &contactAddress;
}

int contactActiveMaxBytes() {
  if (contactEditField == 0) return 64;
  if (contactEditField == 1) return 32;
  if (contactEditField == 2) return 96;
  return 160;
}

String contactFieldName(int field) {
  if (field == 0) return T("ИМЯ", "NAME");
  if (field == 1) return T("ТЕЛЕФОН", "PHONE");
  if (field == 2) return "E-MAIL";
  return T("ИНФОРМАЦИЯ", "INFORMATION");
}

void contactSetRecommendedMode() {
  if (contactEditField == 1) contactInputMode = INPUT_NUMBERS;
  else if (contactEditField == 2) contactInputMode = INPUT_LAT_LOWER;
  else contactInputMode = systemLanguage == LANG_RU ? INPUT_RUS_LOWER : INPUT_LAT_LOWER;
  contactCursor = contactActiveField()->length();
  contactMultiActive = false;
}

void drawContactEditorText() {
  const int textWidth = 100;
  tft.fillRect(0, 36, textWidth, 70, COLOR_BG);
  String* value = contactActiveField();
  if (contactCursor < 0) contactCursor = 0;
  if (contactCursor > value->length()) contactCursor = value->length();
  const int cols = 16, rows = 5, x0 = 3, y0 = 48, lineH = 12;

  int cursorGlyph = sharedCursorGlyphIndex(*value, contactCursor);
  int totalGlyphs = utf8Count(*value);
  int firstGlyph = 0;
  if (cursorGlyph >= cols * rows) firstGlyph = (cursorGlyph / cols - rows + 1) * cols;
  if (firstGlyph < 0) firstGlyph = 0;

  int i = sharedCursorByteForGlyph(*value, firstGlyph);
  int glyphNo = firstGlyph, col = 0, line = 0;
  while (i < value->length() && line < rows) {
    if (glyphNo == cursorGlyph) {
      int cx = x0 + col * 6;
      int cy = y0 + line * lineH - 10;
      tft.drawFastVLine(cx, cy, 10, COLOR_ACCENT);
    }
    String g = utf8GlyphAt(*value, i);
    drawUtf8(x0 + col * 6, y0 + line * lineH, g, COLOR_TEXT, u8g2_font_6x12_t_cyrillic);
    col++; glyphNo++; i = utf8NextIndex(*value, i);
    if (col >= cols) { col = 0; line++; }
  }
  if (cursorGlyph >= glyphNo && line < rows) {
    int cx = x0 + col * 6;
    int cy = y0 + line * lineH - 10;
    if (cx > 96) { cx = x0; cy += lineH; }
    if (cy < 106) tft.drawFastVLine(cx, cy, 10, COLOR_ACCENT);
  }
}

void drawContactEditorHelp() {
  const int helpX = 103;
  tft.fillRect(helpX, 18, 57, 89, COLOR_BG);
  tft.drawFastVLine(helpX - 2, 18, 89, COLOR_GRAY);
  int x = helpX + 2, y = 24;
  auto line = [&](const String& text) { drawUtf8(x, y, text, COLOR_YELLOW, u8g2_font_5x7_t_cyrillic); y += 7; };

  if (contactInputMode == INPUT_RUS_LOWER || contactInputMode == INPUT_RUS_UPPER) {
    bool upper = contactInputMode == INPUT_RUS_UPPER;
    line("1 .,!?" );
    line(upper ? "2 АБВГ" : "2 абвг");
    line(upper ? "3 ДЕЁЖЗ" : "3 деёжз");
    line(upper ? "4 ИЙКЛ" : "4 ийкл");
    line(upper ? "5 МНОП" : "5 мноп");
    line(upper ? "6 РСТУ" : "6 рсту");
    line(upper ? "7 ФХЦЧ" : "7 фхцч");
    line(upper ? "8 ШЩЪЫ" : "8 шщъы");
    line(upper ? "9 ЬЭЮЯ" : "9 ьэюя");
    line(T("0 пробел", "0 space"));
    if (contactEditField == 2) line("* @ . _ - +");
    else line("* @ . _ -");
    line(T("# поле", "# next"));
  } else if (contactInputMode == INPUT_NAV) {
    line(T("НАВИГАЦИЯ", "NAVIGATION"));
    line(T("4 курсор <", "4 cursor <"));
    line(T("6 курсор >", "6 cursor >"));
    line(T("2 строка ^", "2 row up"));
    line(T("8 строка v", "8 row down"));
    line(T("A режим", "A mode"));
    line(T("B удалить", "B delete"));
    line(T("# поле", "# next"));
    line(T("C сохранить", "C save"));
    line(T("D отмена", "D cancel"));
  } else if (contactInputMode == INPUT_NUMBERS) {
    line(T("ЦИФРЫ 123", "NUMBERS 123"));
    line(T("0..9 цифры", "0..9 digits"));
    if (contactEditField == 2) line("* @ . _ - +");
    else line("* @ . _ -");
    line(T("A режим", "A mode"));
    line(T("B удалить", "B delete"));
    line(T("# поле", "# next"));
    line(T("C сохранить", "C save"));
    line(T("D отмена", "D cancel"));
  } else {
    bool upper = contactInputMode == INPUT_LAT_UPPER;
    line("1 .,!?" );
    line(upper ? "2 ABC" : "2 abc");
    line(upper ? "3 DEF" : "3 def");
    line(upper ? "4 GHI" : "4 ghi");
    line(upper ? "5 JKL" : "5 jkl");
    line(upper ? "6 MNO" : "6 mno");
    line(upper ? "7 PQRS" : "7 pqrs");
    line(upper ? "8 TUV" : "8 tuv");
    line(upper ? "9 WXYZ" : "9 wxyz");
    line(T("0 пробел", "0 space"));
    if (contactEditField == 2) line("* @ . _ - +");
    else line("* @ . _ -");
    line(T("# поле", "# next"));
  }
}

void drawContactEditor() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(4, 13, contactEditingExisting ? T("РЕДАКТ КОНТАКТА", "EDIT CONTACT") : T("НОВЫЙ КОНТАКТ", "NEW CONTACT"), COLOR_TEXT);
  uiTextSmall(133, 13, inputModeName(contactInputMode), COLOR_YELLOW);

  String label = String(contactEditField + 1) + "/4 " + contactFieldName(contactEditField);
  drawUtf8(3, 31, label, COLOR_ACCENT, u8g2_font_5x7_t_cyrillic);
  drawContactEditorText();
  drawContactEditorHelp();

  tft.fillRect(0, 107, 160, 21, COLOR_BG);
  tft.drawFastHLine(0, 107, 160, COLOR_GRAY);
  if (contactInputMode == INPUT_NAV) uiTextSmall(2, 116, T("2/8/4/6 КУРСОР  A РЕЖ", "2/8/4/6 CURSOR A MODE"), COLOR_GRAY);
  else uiTextSmall(2, 116, T("A РЕЖ B УДАЛ # ПОЛЕ", "A MODE B DEL # NEXT"), COLOR_GRAY);
  uiTextSmall(2, 125, T("C СОХР  D ОТМЕНА", "C SAVE  D CANCEL"), COLOR_GRAY);
}

void contactMultiTap(char key) {
  String* value = contactActiveField();
  if (contactInputMode == INPUT_NAV) return;
  if (contactInputMode == INPUT_NUMBERS && key >= '0' && key <= '9') {
    String glyph(key);
    sharedInsert(*value, contactCursor, glyph, contactActiveMaxBytes());
    contactMultiActive = false;
    drawContactEditorText();
    return;
  }

  String set;
  if (contactEditField == 2 && key == '*') set = "@._-+";
  else if (contactInputMode == INPUT_NUMBERS && key == '*') set = "+-().";
  else set = getCharacterSet(key, contactInputMode);
  if (!set.length()) return;

  unsigned long now = millis();
  bool sameKey = contactMultiActive && key == contactLastKey && now - contactLastTime < 900;
  int count = utf8Count(set);
  if (sameKey && contactCursor > 0) {
    contactMultiIndex = (contactMultiIndex + 1) % max(1, count);
    String glyph = utf8GlyphAtNumber(set, contactMultiIndex);
    utf8ReplacePrevious(*value, contactCursor, glyph);
  } else {
    contactMultiIndex = 0;
    String glyph = utf8GlyphAtNumber(set, 0);
    sharedInsert(*value, contactCursor, glyph, contactActiveMaxBytes());
  }
  contactLastKey = key;
  contactLastTime = now;
  contactMultiActive = true;
  drawContactEditorText();
}

void contactEditorKey(char key) {
  if (key == 'A') {
    contactMultiActive = false;
    if (contactInputMode == INPUT_LAT_LOWER) contactInputMode = INPUT_LAT_UPPER;
    else if (contactInputMode == INPUT_LAT_UPPER) contactInputMode = INPUT_RUS_LOWER;
    else if (contactInputMode == INPUT_RUS_LOWER) contactInputMode = INPUT_RUS_UPPER;
    else if (contactInputMode == INPUT_RUS_UPPER) contactInputMode = INPUT_NUMBERS;
    else if (contactInputMode == INPUT_NUMBERS) contactInputMode = INPUT_NAV;
    else contactInputMode = INPUT_LAT_LOWER;
    drawContactEditor(); return;
  }

  if (contactInputMode == INPUT_NAV && (key == '2' || key == '8' || key == '4' || key == '6')) {
    contactMultiActive = false;
    String* value = contactActiveField();
    if (key == '4') sharedCursorMoveHorizontal(*value, contactCursor, -1);
    else if (key == '6') sharedCursorMoveHorizontal(*value, contactCursor, 1);
    else if (key == '2') sharedCursorMoveVertical(*value, contactCursor, 16, -1);
    else if (key == '8') sharedCursorMoveVertical(*value, contactCursor, 16, 1);
    drawContactEditorText(); return;
  }

  if (key == 'B') {
    contactMultiActive = false;
    String* value = contactActiveField();
    if (sharedBackspace(*value, contactCursor)) drawContactEditorText();
    return;
  }
  if (key == '#') {
    contactMultiActive = false;
    contactEditField = (contactEditField + 1) % 4;
    contactSetRecommendedMode();
    drawContactEditor(); return;
  }
  if (key == 'C') {
    contactMultiActive = false;
    if (!contactName.length()) {
      showMessage(T("УКАЖИТЕ ИМЯ", "ENTER A NAME"), "", COLOR_RED, 700);
      currentScreen = SCREEN_CONTACT_EDIT; drawContactEditor(); return;
    }
    if (!saveContact(contactEditPath, contactName, contactPhone, contactEmail, contactAddress)) {
      showMessage(T("ОШИБКА СОХРАНЕНИЯ", "SAVE ERROR"), "", COLOR_RED, 700);
      currentScreen = SCREEN_CONTACT_EDIT; drawContactEditor(); return;
    }
    showMessage(T("КОНТАКТ СОХРАНЁН", "CONTACT SAVED"), "", COLOR_GREEN, 550);
    refreshContactList();
    for (int i = 0; i < contactCount; i++) if (contactPaths[i] == contactEditPath) { contactSelected = i; break; }
    currentScreen = SCREEN_CONTACT_VIEW;
    loadContact(contactEditPath, contactName, contactPhone, contactEmail, contactAddress);
    drawContactView();
    return;
  }
  if (key == 'D') {
    contactMultiActive = false;
    if (contactEditingExisting) {
      loadContact(contactEditPath, contactName, contactPhone, contactEmail, contactAddress);
      currentScreen = SCREEN_CONTACT_VIEW;
      drawContactView();
    } else {
      currentScreen = SCREEN_CONTACTS;
      refreshContactList();
      drawContactsScreen();
    }
    return;
  }
  contactMultiTap(key);
}

void deleteSelectedContact() {
  if (contactCount <= 0 || contactSelected < 0 || contactSelected >= contactCount) return;
  String path = contactPaths[contactSelected];
  if (!LittleFS.remove(path)) {
    showMessage(T("ОШИБКА УДАЛЕНИЯ", "DELETE ERROR"), "", COLOR_RED, 650);
  } else {
    showMessage(T("КОНТАКТ УДАЛЁН", "CONTACT DELETED"), "", COLOR_GREEN, 450);
  }
  refreshContactList();
  currentScreen = SCREEN_CONTACTS;
  drawContactsScreen();
}




String securePinDigest(const String& pin) {
  String tagged = String("PPC-SECURE-PIN:") + pin;
  return sha256Hex((const uint8_t*)tagged.c_str(), tagged.length());
}

void secureDeriveKey(const String& pin, const uint8_t salt[16], uint8_t key[32]) {


  uint8_t buffer[64];
  int pinLen = pin.length();
  if (pinLen > 16) pinLen = 16;

  memcpy(buffer, salt, 16);
  memcpy(buffer + 16, pin.c_str(), pinLen);
  const char* label = "PPC-AES256";
  int labelLen = strlen(label);
  memcpy(buffer + 16 + pinLen, label, labelLen);

  PpcSha256Ctx ctx;
  ppcShaInit(ctx);
  ppcShaUpdate(ctx, buffer, 16 + pinLen + labelLen);
  ppcShaFinal(ctx, key);

  for (int round = 0; round < 2048; round++) {
    uint8_t tmp[64];
    memcpy(tmp, key, 32);
    memcpy(tmp + 32, salt, 16);
    memcpy(tmp + 48, pin.c_str(), pinLen);

    ppcShaInit(ctx);
    ppcShaUpdate(ctx, tmp, 48 + pinLen);
    ppcShaFinal(ctx, key);
  }
}

static void secureRandomBytes(uint8_t* out, size_t len) {
  size_t pos = 0;
  while (pos < len) {
    uint32_t r = esp_random();
    for (int i = 0; i < 4 && pos < len; i++, pos++) {
      out[pos] = (uint8_t)(r >> (i * 8));
    }
  }
}

bool secureEncryptFile(const String& path, const String& plain, const String& pin) {
  const uint8_t header[8] = {'P','P','C','S','E','C','0','1'};
  uint8_t salt[16], iv[16], key[32];
  secureRandomBytes(salt, sizeof(salt));
  secureRandomBytes(iv, sizeof(iv));
  secureDeriveKey(pin, salt, key);

  size_t plainLen = plain.length();
  size_t paddedLen = ((plainLen / 16) + 1) * 16;
  uint8_t* data = (uint8_t*)malloc(paddedLen);
  if (!data) return false;

  if (plainLen) memcpy(data, plain.c_str(), plainLen);
  uint8_t pad = (uint8_t)(paddedLen - plainLen);
  for (size_t i = plainLen; i < paddedLen; i++) data[i] = pad;

  uint8_t ivWork[16];
  memcpy(ivWork, iv, 16);

  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  int rc = mbedtls_aes_setkey_enc(&aes, key, 256);
  if (rc == 0) {
    rc = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, paddedLen, ivWork, data, data);
  }
  mbedtls_aes_free(&aes);

  if (rc != 0) {
    free(data);
    return false;
  }

  File file = LittleFS.open(path, "w");
  if (!file) {
    free(data);
    return false;
  }

  bool ok = true;
  ok &= file.write(header, sizeof(header)) == sizeof(header);
  ok &= file.write(salt, sizeof(salt)) == sizeof(salt);
  ok &= file.write(iv, sizeof(iv)) == sizeof(iv);
  ok &= file.write(data, paddedLen) == paddedLen;
  file.close();
  free(data);
  return ok;
}

bool secureDecryptFile(const String& path, String& plain, const String& pin) {
  plain = "";
  File file = LittleFS.open(path, "r");
  if (!file) return false;

  size_t total = file.size();
  if (total < 56 || ((total - 40) % 16) != 0) {
    file.close();
    return false;
  }

  uint8_t header[8];
  uint8_t expected[8] = {'P','P','C','S','E','C','0','1'};
  if (file.read(header, 8) != 8 || memcmp(header, expected, 8) != 0) {
    file.close();
    return false;
  }

  uint8_t salt[16], iv[16], key[32];
  if (file.read(salt, 16) != 16 || file.read(iv, 16) != 16) {
    file.close();
    return false;
  }

  size_t cipherLen = total - 40;
  uint8_t* data = (uint8_t*)malloc(cipherLen);
  if (!data) {
    file.close();
    return false;
  }
  if ((size_t)file.read(data, cipherLen) != cipherLen) {
    file.close();
    free(data);
    return false;
  }
  file.close();

  secureDeriveKey(pin, salt, key);
  uint8_t ivWork[16];
  memcpy(ivWork, iv, 16);

  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  int rc = mbedtls_aes_setkey_dec(&aes, key, 256);
  if (rc == 0) {
    rc = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, cipherLen, ivWork, data, data);
  }
  mbedtls_aes_free(&aes);

  if (rc != 0) {
    free(data);
    return false;
  }

  uint8_t pad = data[cipherLen - 1];
  if (pad < 1 || pad > 16 || pad > cipherLen) {
    free(data);
    return false;
  }
  for (uint8_t i = 0; i < pad; i++) {
    if (data[cipherLen - 1 - i] != pad) {
      free(data);
      return false;
    }
  }

  size_t plainLen = cipherLen - pad;
  plain.reserve(plainLen);
  for (size_t i = 0; i < plainLen; i++) plain += (char)data[i];
  free(data);

  if (plain.length() > MAX_NOTE_BYTES) plain = plain.substring(0, MAX_NOTE_BYTES);
  return true;
}

void beginSecureVault(SecureVaultTarget target) {
  secureVaultTarget = target;
  securePinInput = "";
  securePinFirst = "";
  secureSessionPin = "";
  secureVaultUnlocked = false;

  securePinStage = securePinHash.length() ? 0 : 1;
  currentScreen = SCREEN_SECURE_PIN;
  drawSecurePinScreen();
}

void openSecureVaultTarget() {
  if (secureVaultTarget == VAULT_TARGET_CONTACTS) {
    contactSelected = 0;
    contactTop = 0;
    refreshContactList();
    currentScreen = SCREEN_CONTACTS;
    drawContactsScreen();
  } else {
    refreshSecureNoteList();
    currentScreen = SCREEN_SECURE_NOTES;
    drawSecureNotesScreen();
  }
}

void drawSecurePinScreen() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, secureVaultTarget == VAULT_TARGET_CONTACTS ? T("АДРЕСНАЯ КНИГА", "ADDRESS BOOK") : T("ЗАШИФРОВАННЫЕ", "ENCRYPTED NOTES"), COLOR_TEXT);

  if (securePinStage == 0) {
    drawUiCentered(T("ВВЕДИТЕ PIN", "ENTER PIN"), 43, COLOR_TEXT, u8g2_font_6x12_t_cyrillic, 6);
  } else if (securePinStage == 1) {
    drawUiCentered(T("СОЗДАЙТЕ PIN 4-6", "CREATE PIN 4-6"), 43, COLOR_TEXT, u8g2_font_6x12_t_cyrillic, 6);
  } else {
    drawUiCentered(T("ПОВТОРИТЕ PIN", "CONFIRM PIN"), 43, COLOR_TEXT, u8g2_font_6x12_t_cyrillic, 6);
  }

  String mask = "";
  for (int i = 0; i < securePinInput.length(); i++) mask += '*';
  if (!mask.length()) mask = "_";
  drawUiCentered(mask, 69, COLOR_YELLOW, u8g2_font_6x12_t_cyrillic, 6);

  if (securePinStage == 1) {
    uiTextSmall(15, 89, T("PIN ШИФРУЕТ СОДЕРЖИМОЕ", "PIN ENCRYPTS CONTENT"), COLOR_GRAY);
  } else if (secureVaultTarget == VAULT_TARGET_CONTACTS) {
    uiTextSmall(21, 89, T("AES-256 КОНТАКТЫ", "AES-256 CONTACTS"), COLOR_GRAY);
  } else {
    uiTextSmall(18, 89, T("AES-256 ЗАЩИЩЁННАЯ ЗАПИСЬ", "AES-256 ENCRYPTED NOTE"), COLOR_GRAY);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(5, 118, T("0-9 ВВОД  B УДАЛ  C СБРОС", "0-9 TYPE  B DEL  C CLEAR"), COLOR_GRAY);
  uiTextSmall(17, 126, T("A/# OK       D НАЗАД", "A/# OK       D BACK"), COLOR_GRAY);
}

void securePinKey(char key) {
  if (key >= '0' && key <= '9') {
    if (securePinInput.length() < 6) {
      securePinInput += key;
      drawSecurePinScreen();
    }
    return;
  }
  if (key == 'B') {
    if (securePinInput.length()) securePinInput.remove(securePinInput.length() - 1);
    drawSecurePinScreen();
    return;
  }
  if (key == 'C') {
    securePinInput = "";
    drawSecurePinScreen();
    return;
  }
  if (key == 'D') {
    closeSecureVault();
    currentScreen = SCREEN_RECORDS_MENU;
    drawRecordsMenu();
    return;
  }
  if (key != 'A' && key != '#') return;

  if (securePinInput.length() < 4 || securePinInput.length() > 6) {
    showMessage(T("PIN: 4-6 ЦИФР", "PIN: 4-6 DIGITS"), "", COLOR_RED, 650);
    drawSecurePinScreen();
    return;
  }

  if (securePinStage == 0) {
    if (securePinDigest(securePinInput) != securePinHash) {
      securePinInput = "";
      showMessage(T("НЕВЕРНЫЙ PIN", "WRONG PIN"), T("ПОПРОБУЙТЕ ЕЩЁ", "TRY AGAIN"), COLOR_RED, 700);
      drawSecurePinScreen();
      return;
    }
    secureSessionPin = securePinInput;
    securePinInput = "";
    secureVaultUnlocked = true;
    openSecureVaultTarget();
    return;
  }

  if (securePinStage == 1) {
    securePinFirst = securePinInput;
    securePinInput = "";
    securePinStage = 2;
    drawSecurePinScreen();
    return;
  }

  if (securePinInput != securePinFirst) {
    securePinInput = "";
    securePinFirst = "";
    securePinStage = 1;
    showMessage(T("PIN НЕ СОВПАЛ", "PIN MISMATCH"), T("СОЗДАЙТЕ ЕЩЁ РАЗ", "CREATE AGAIN"), COLOR_RED, 750);
    drawSecurePinScreen();
    return;
  }

  secureSessionPin = securePinInput;
  securePinHash = securePinDigest(secureSessionPin);
  preferences.putString("secPinHash", securePinHash);
  securePinInput = "";
  securePinFirst = "";
  secureVaultUnlocked = true;
  openSecureVaultTarget();
}

void refreshSecureNoteList() {
  secureNoteCount = 0;
  for (int i = 1; i <= MAX_SECURE_NOTES; i++) {
    char path[24];
    snprintf(path, sizeof(path), "/secure/SEC%02d.DAT", i);
    if (LittleFS.exists(path)) {
      secureNoteFiles[secureNoteCount++] = String(path);
      if (secureNoteCount >= MAX_SECURE_NOTES) break;
    }
  }

  if (secureNoteSelected >= secureNoteCount) secureNoteSelected = secureNoteCount - 1;
  if (secureNoteSelected < 0) secureNoteSelected = 0;
  if (secureNoteTop > secureNoteSelected) secureNoteTop = secureNoteSelected;
  secureDeleteArmed = false;
}

void drawSecureNotesScreen() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("ЗАШИФРОВАННЫЕ", "ENCRYPTED"), COLOR_TEXT);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT, COLOR_SELECT);
  tft.setCursor(130, 5);
  tft.print(secureNoteCount);
  tft.print(" F");

  drawSecureNoteRows();
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(1, 123, T("2/8 ВЫБ A ОТКР B НОВ C УДАЛ", "2/8 SEL A OPEN B NEW C DEL"), COLOR_GRAY);
}

void drawSecureNoteRows() {
  tft.fillRect(0, 20, 160, 87, COLOR_BG);
  if (secureNoteCount == 0) {
    uiText(30, 55, T("НЕТ ЗАШИФР. ЗАПИСЕЙ", "NO ENCRYPTED NOTES"), COLOR_YELLOW);
    uiText(34, 72, T("B = НОВАЯ", "B = NEW"), COLOR_GRAY);
    return;
  }

  for (int row = 0; row < SECURE_VISIBLE_ROWS; row++) {
    int index = secureNoteTop + row;
    if (index >= secureNoteCount) break;
    int y = 25 + row * 20;
    uint16_t c = COLOR_GRAY;
    if (index == secureNoteSelected) {
      tft.fillRoundRect(3, y - 3, 154, 18, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    String name = secureNoteFiles[index];
    int slash = name.lastIndexOf('/');
    if (slash >= 0) name = name.substring(slash + 1);
    drawUtf8(6, y + 8, name, c, u8g2_font_6x12_t_cyrillic);
    drawUtf8(91, y + 7, "AES-256", c, u8g2_font_5x7_t_cyrillic);
  }
}

String createNewSecureNote() {
  if (!secureVaultUnlocked || !secureSessionPin.length()) return "";
  if (!LittleFS.exists("/secure")) LittleFS.mkdir("/secure");

  for (int i = 1; i <= MAX_SECURE_NOTES; i++) {
    char path[24];
    snprintf(path, sizeof(path), "/secure/SEC%02d.DAT", i);
    if (!LittleFS.exists(path)) {
      if (!secureEncryptFile(String(path), "", secureSessionPin)) return "";
      return String(path);
    }
  }
  return "";
}

void openSecureNote(const String& path) {
  if (!secureVaultUnlocked || !secureSessionPin.length()) return;

  String plain;
  if (!secureDecryptFile(path, plain, secureSessionPin)) {
    showMessage(T("НЕ УДАЛОСЬ РАСШИФРОВАТЬ", "DECRYPT FAILED"), T("ФАЙЛ ПОВРЕЖДЁН?", "FILE CORRUPTED?"), COLOR_RED, 800);
    drawSecureNotesScreen();
    return;
  }

  noteText = plain;
  currentNotePath = path;
  editCursor = noteText.length();
  editorTopLine = 0;
  noteDirty = false;
  editorMode = systemLanguage == LANG_RU ? INPUT_RUS_LOWER : INPUT_LAT_LOWER;
  editorMultiActive = false;
  secureEditorMode = true;
  editorReturnScreen = SCREEN_SECURE_NOTES;
  currentScreen = SCREEN_EDITOR;
  drawEditor();
}

void deleteSelectedSecureNote() {
  if (secureNoteCount <= 0) return;
  LittleFS.remove(secureNoteFiles[secureNoteSelected]);
  refreshSecureNoteList();
  drawSecureNotesScreen();
}

void closeSecureVault() {
  secureVaultUnlocked = false;
  secureSessionPin = "";
  securePinInput = "";
  securePinFirst = "";
  secureEditorMode = false;



  contactName = "";
  contactPhone = "";
  contactEmail = "";
  contactAddress = "";
  contactEditPath = "";
  for (int i = 0; i < MAX_CONTACTS; i++) {
    contactNames[i] = "";
    contactPaths[i] = "";
  }
  contactCount = 0;
  contactSelected = 0;
  contactTop = 0;
}

void secureNotesKey(char key) {
  if (key != 'C') secureDeleteArmed = false;

  if (key == '2' && secureNoteSelected > 0) {
    secureNoteSelected--;
    if (secureNoteSelected < secureNoteTop) secureNoteTop = secureNoteSelected;
    drawSecureNoteRows();
    return;
  }
  if (key == '8' && secureNoteSelected < secureNoteCount - 1) {
    secureNoteSelected++;
    if (secureNoteSelected >= secureNoteTop + SECURE_VISIBLE_ROWS) {
      secureNoteTop = secureNoteSelected - SECURE_VISIBLE_ROWS + 1;
    }
    drawSecureNoteRows();
    return;
  }
  if (key == 'A') {
    if (secureNoteCount > 0) openSecureNote(secureNoteFiles[secureNoteSelected]);
    return;
  }
  if (key == 'B') {
    String path = createNewSecureNote();
    if (path.length()) {
      refreshSecureNoteList();
      openSecureNote(path);
    }
    return;
  }
  if (key == 'C') {
    if (secureNoteCount == 0) return;
    if (!secureDeleteArmed) {
      secureDeleteArmed = true;
      tft.fillRect(0, 109, 160, 19, COLOR_BG);
      uiTextSmall(6, 123, T("C ЕЩЁ РАЗ = УДАЛИТЬ", "PRESS C AGAIN = DELETE"), COLOR_RED);
    } else {
      deleteSelectedSecureNote();
    }
    return;
  }
  if (key == 'D') {
    closeSecureVault();
    currentScreen = SCREEN_RECORDS_MENU;
    drawRecordsMenu();
  }
}




void refreshNoteList() {
  noteCount = 0;
  for (int i = 1; i <= MAX_NOTES; i++) {
    char path[16]; snprintf(path, sizeof(path), "/NOTE%02d.TXT", i);
    if (LittleFS.exists(path)) {
      noteFiles[noteCount++] = String(path);
      if (noteCount >= MAX_NOTES) break;
    }
  }
  if (noteSelected >= noteCount) noteSelected = noteCount - 1;
  if (noteSelected < 0) noteSelected = 0;
  if (noteTop > noteSelected) noteTop = noteSelected;
  noteDeleteArmed = false;
}

String getNotePreview(String path) {
  File file = LittleFS.open(path, "r");
  if (!file) return "";
  String result = "";
  while (file.available() && result.length() < 80) {
    char c = file.read();
    if (c == '\n' || c == '\r') break;
    result += c;
  }
  file.close();
  return utf8Truncate(result, 12);
}

void drawNotesScreen() {
  tft.fillScreen(COLOR_BG); tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("ОБЫЧНЫЕ ЗАМЕТКИ", "PLAIN NOTES"), COLOR_TEXT);
  tft.setTextSize(1); tft.setTextColor(COLOR_TEXT); tft.setCursor(128, 5); tft.print(noteCount); tft.print(" F");
  drawNoteRows();
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(1, 123, T("2/8 ВЫБ A ОТКР B НОВ C УДАЛ", "2/8 SEL A OPEN B NEW C DEL"), COLOR_GRAY);
}

void drawNoteRows() {
  tft.fillRect(0, 20, 160, 87, COLOR_BG);
  if (noteCount == 0) {
    uiText(45, 58, T("НЕТ ЗАМЕТОК", "NO NOTES"), COLOR_YELLOW);
    uiText(34, 72, T("B = НОВАЯ", "B = NEW"), COLOR_GRAY);
    return;
  }
  for (int row = 0; row < NOTE_VISIBLE_ROWS; row++) {
    int index = noteTop + row; if (index >= noteCount) break;
    int y = 25 + row * 20; uint16_t c = COLOR_GRAY;
    if (index == noteSelected) { tft.fillRoundRect(3, y - 3, 154, 18, 3, COLOR_SELECT); c = COLOR_TEXT; }
    String name = noteFiles[index]; if (name.startsWith("/")) name = name.substring(1);
    drawUtf8(5, y + 8, utf8Truncate(name, 11), c, u8g2_font_6x12_t_cyrillic);
    String preview = getNotePreview(noteFiles[index]);
    if (preview.length()) drawUtf8(78, y + 7, utf8Truncate(preview, 15), c, u8g2_font_5x7_t_cyrillic);
  }
}

String createNewNote() {
  for (int i = 1; i <= MAX_NOTES; i++) {
    char path[16]; snprintf(path, sizeof(path), "/NOTE%02d.TXT", i);
    if (!LittleFS.exists(path)) {
      File file = LittleFS.open(path, "w");
      if (!file) return "";
      file.close(); return String(path);
    }
  }
  return "";
}

void openNote(String path) {
  secureEditorMode = false;
  File file = LittleFS.open(path, "r"); if (!file) return;
  noteText = file.readString(); file.close();
  if (noteText.length() > MAX_NOTE_BYTES) noteText = noteText.substring(0, MAX_NOTE_BYTES);
  currentNotePath = path;
  editCursor = noteText.length();
  editorTopLine = 0;
  noteDirty = false;
  editorMode = INPUT_LAT_LOWER;
  editorMultiActive = false;
  currentScreen = SCREEN_EDITOR;
  drawEditor();
}

void deleteSelectedNote() {
  if (noteCount <= 0) return;
  LittleFS.remove(noteFiles[noteSelected]);
  refreshNoteList(); drawNotesScreen();
}

void notesKey(char key) {
  if (key != 'C') noteDeleteArmed = false;
  if (key == '2' && noteSelected > 0) {
    noteSelected--; if (noteSelected < noteTop) noteTop = noteSelected;
    drawNoteRows(); return;
  }
  if (key == '8' && noteSelected < noteCount - 1) {
    noteSelected++; if (noteSelected >= noteTop + NOTE_VISIBLE_ROWS) noteTop = noteSelected - NOTE_VISIBLE_ROWS + 1;
    drawNoteRows(); return;
  }
  if (key == 'A') {
    if (noteCount > 0) {
      editorReturnScreen = SCREEN_NOTES;
      openNote(noteFiles[noteSelected]);
    }
    return;
  }
  if (key == 'B') {
    String p = createNewNote();
    if (p.length()) {
      refreshNoteList();
      editorReturnScreen = SCREEN_NOTES;
      openNote(p);
    }
    return;
  }
  if (key == 'C') {
    if (noteCount == 0) return;
    if (!noteDeleteArmed) {
      noteDeleteArmed = true;
      tft.fillRect(0, 109, 160, 19, COLOR_BG);
      uiTextSmall(6, 123, T("C ЕЩЁ РАЗ = УДАЛИТЬ", "PRESS C AGAIN = DELETE"), COLOR_RED);
    } else deleteSelectedNote();
    return;
  }
  if (key == 'D') { currentScreen = SCREEN_RECORDS_MENU; drawRecordsMenu(); }
}




void getVisualPosition(int byteIndex, int& line, int& column) {
  line = 0; column = 0;
  if (byteIndex > noteText.length()) byteIndex = noteText.length();
  int i = 0;
  while (i < byteIndex) {
    if (noteText[i] == '\n') {
      line++; column = 0; i++;
    } else {
      i = utf8NextIndex(noteText, i);
      column++;
      if (column >= EDIT_COLS) { line++; column = 0; }
    }
  }
}

int indexForVisualPosition(int targetLine, int targetColumn) {
  if (targetLine < 0) return 0;
  int line = 0, col = 0, i = 0, best = 0;
  while (i <= noteText.length()) {
    if (line == targetLine) {
      best = i;
      if (col >= targetColumn) return i;
    }
    if (line > targetLine || i == noteText.length()) return best;
    if (noteText[i] == '\n') {
      if (line == targetLine) return i;
      i++; line++; col = 0;
    } else {
      i = utf8NextIndex(noteText, i);
      col++;
      if (col >= EDIT_COLS) { line++; col = 0; }
    }
  }
  return best;
}

void moveCursorHorizontal(int delta) {
  editorMultiActive = false;
  if (delta < 0) editCursor = utf8PrevIndex(noteText, editCursor);
  else if (delta > 0) editCursor = utf8NextIndex(noteText, editCursor);
  drawEditorBody(); drawEditorFooter();
}

void moveCursorVertical(int delta) {
  editorMultiActive = false;
  int line, col; getVisualPosition(editCursor, line, col);
  int target = line + delta; if (target < 0) target = 0;
  editCursor = indexForVisualPosition(target, col);
  drawEditorBody(); drawEditorFooter();
}

void insertEditorGlyph(const String& glyph) {
  if (noteText.length() + glyph.length() > MAX_NOTE_BYTES) return;
  utf8InsertAt(noteText, editCursor, glyph);
  editCursor += glyph.length();
  noteDirty = true;
}

void backspaceEditor() {
  editorMultiActive = false;
  if (editCursor <= 0 || !noteText.length()) return;
  int prev = utf8PrevIndex(noteText, editCursor);
  noteText.remove(prev, editCursor - prev);
  editCursor = prev;
  noteDirty = true;
  drawEditorBody(); drawEditorFooter();
}

void saveCurrentNote() {
  if (!currentNotePath.length()) return;

  if (secureEditorMode) {
    if (!secureVaultUnlocked || !secureSessionPin.length()) return;
    if (!secureEncryptFile(currentNotePath, noteText, secureSessionPin)) {
      showMessage(T("ОШИБКА ШИФРОВАНИЯ", "ENCRYPT ERROR"), T("ФАЙЛ НЕ СОХРАНЁН", "FILE NOT SAVED"), COLOR_RED, 700);
      drawEditor();
      return;
    }
  } else {
    File file = LittleFS.open(currentNotePath, "w"); if (!file) return;
    file.print(noteText); file.close();
  }

  noteDirty = false;
  drawEditorHeader(); drawEditorFooter();
}

void drawEditorHeader() {
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  String name = currentNotePath;
  if (name.startsWith("/secure/")) name = name.substring(8);
  else if (name.startsWith("/")) name = name.substring(1);
  if (secureEditorMode) name = String("[AES] ") + name;




  int glyphs = utf8Count(name);
  if (glyphs <= 38) {
    drawUtf8(3, 12, name, COLOR_TEXT, u8g2_font_4x6_t_cyrillic);
  } else {
    String first = "";
    String second = "";
    int i = 0, n = 0;
    while (i < name.length()) {
      String g = utf8GlyphAt(name, i);
      if (n < 38) first += g;
      else second += g;
      n++;
      i = utf8NextIndex(name, i);
    }
    drawUtf8(3, 7, first, COLOR_TEXT, u8g2_font_4x6_t_cyrillic);
    drawUtf8(3, 14, second, COLOR_TEXT, u8g2_font_4x6_t_cyrillic);
  }
}

void drawEditorHelp() {
  tft.fillRect(EDIT_HELP_X, 18, 57, 89, COLOR_BG);
  tft.drawFastVLine(EDIT_HELP_X - 2, 18, 89, COLOR_GRAY);
  int x = EDIT_HELP_X + 2, y = 24;
  auto line = [&](const String& s) { drawUtf8(x, y, s, COLOR_YELLOW, u8g2_font_5x7_t_cyrillic); y += 7; };
  if (editorMode == INPUT_RUS_LOWER || editorMode == INPUT_RUS_UPPER) {
    line("1 .,!?"); line(editorMode == INPUT_RUS_UPPER ? "2 АБВГ" : "2 абвг");
    line(editorMode == INPUT_RUS_UPPER ? "3 ДЕЁЖЗ" : "3 деёжз"); line(editorMode == INPUT_RUS_UPPER ? "4 ИЙКЛ" : "4 ийкл");
    line(editorMode == INPUT_RUS_UPPER ? "5 МНОП" : "5 мноп"); line(editorMode == INPUT_RUS_UPPER ? "6 РСТУ" : "6 рсту");
    line(editorMode == INPUT_RUS_UPPER ? "7 ФХЦЧ" : "7 фхцч"); line(editorMode == INPUT_RUS_UPPER ? "8 ШЩЪЫ" : "8 шщъы");
    line(editorMode == INPUT_RUS_UPPER ? "9 ЬЭЮЯ" : "9 ьэюя"); line(T("0 пробел", "0 space")); line("* @ . _ -"); line(T("# строка", "# newline"));
  } else if (editorMode == INPUT_NAV) {
    line(T("НАВИГАЦИЯ", "NAVIGATION")); line(T("2 ВВЕРХ", "2 UP")); line(T("8 ВНИЗ", "8 DOWN")); line(T("4 ВЛЕВО", "4 LEFT")); line(T("6 ВПРАВО", "6 RIGHT"));
    line(T("A РЕЖИМ", "A MODE")); line(T("B УДАЛ", "B DELETE")); line(T("C СОХР", "C SAVE")); line(T("D НАЗАД", "D BACK"));
  } else if (editorMode == INPUT_NUMBERS) {
    line(T("ЦИФРЫ 123", "NUMBERS 123")); line(T("0..9 цифры", "0..9 digits")); line(T("A РЕЖИМ", "A MODE")); line(T("B УДАЛ", "B DELETE")); line(T("C СОХР", "C SAVE")); line(T("D НАЗАД", "D BACK")); line(T("# строка", "# newline"));
  } else {
    bool upper = (editorMode == INPUT_LAT_UPPER);
    line("1 .,!?");
    line(upper ? "2 ABC" : "2 abc");
    line(upper ? "3 DEF" : "3 def");
    line(upper ? "4 GHI" : "4 ghi");
    line(upper ? "5 JKL" : "5 jkl");
    line(upper ? "6 MNO" : "6 mno");
    line(upper ? "7 PQRS" : "7 pqrs");
    line(upper ? "8 TUV" : "8 tuv");
    line(upper ? "9 WXYZ" : "9 wxyz");
    line(T("0 пробел", "0 space")); line("* @ . _ -"); line(T("# строка", "# newline"));
  }
}

void drawEditorBody() {
  int cursorLine, cursorCol; getVisualPosition(editCursor, cursorLine, cursorCol);
  if (cursorLine < editorTopLine) editorTopLine = cursorLine;
  if (cursorLine >= editorTopLine + EDIT_ROWS) editorTopLine = cursorLine - EDIT_ROWS + 1;
  if (editorTopLine < 0) editorTopLine = 0;

  tft.fillRect(0, 18, EDIT_TEXT_WIDTH, 89, COLOR_BG);
  int line = 0, col = 0, i = 0;
  while (i < noteText.length()) {
    if (noteText[i] == '\n') { line++; col = 0; i++; continue; }
    String glyph = utf8GlyphAt(noteText, i);
    if (line >= editorTopLine && line < editorTopLine + EDIT_ROWS) {


      int x = 3 + col * 6;
      int baseline = 29 + (line - editorTopLine) * 12;
      drawUtf8(x, baseline, glyph, COLOR_TEXT, u8g2_font_6x12_t_cyrillic);
    }
    i = utf8NextIndex(noteText, i);
    col++;
    if (col >= EDIT_COLS) { line++; col = 0; }
  }

  if (cursorLine >= editorTopLine && cursorLine < editorTopLine + EDIT_ROWS) {
    int x = 2 + cursorCol * 6;
    int y = 19 + (cursorLine - editorTopLine) * 12;
    tft.drawFastVLine(x, y, 11, COLOR_ACCENT);
  }
}

void drawEditorFooter() {
  tft.fillRect(0, 107, 160, 21, COLOR_BG); tft.drawFastHLine(0, 107, 160, COLOR_GRAY);
  tft.setTextSize(1); tft.setTextColor(noteDirty ? COLOR_YELLOW : COLOR_GREEN);
  tft.setCursor(3, 110); tft.print(noteDirty ? "*" : "S"); tft.print(utf8Count(noteText));
  uiTextSmall(64, 115, T("РЕЖИМ:", "MODE:"), COLOR_GRAY); uiTextSmall(90, 115, inputModeName(editorMode), COLOR_GRAY);
  if (editorMode == INPUT_NAV) uiTextSmall(3, 124, T("2^ 8v 4< 6> A РЕЖИМ", "2^ 8v 4< 6> A MODE"), COLOR_GRAY);
  else uiTextSmall(3, 124, T("A РЕЖ B УДАЛ C СОХР D НАЗ", "A MOD B DEL C SAVE D BACK"), COLOR_GRAY);
}

void drawEditor() {
  tft.fillScreen(COLOR_BG);
  drawEditorHeader(); drawEditorBody(); drawEditorHelp(); drawEditorFooter();
}

void editorMultiTap(char key) {
  if (editorMode == INPUT_NUMBERS && key >= '0' && key <= '9') {
    insertEditorGlyph(String(key));
    editorMultiActive = false;
    drawEditorBody(); drawEditorFooter(); return;
  }

  String set = getCharacterSet(key, editorMode);
  if (!set.length()) return;
  int setCount = utf8Count(set);
  unsigned long now = millis();
  bool sameKey = editorMultiActive && key == editorLastKey && now - editorLastTime < 900;

  if (sameKey && editCursor > 0) {
    editorMultiIndex++;
    if (editorMultiIndex >= setCount) editorMultiIndex = 0;
    String glyph = utf8GlyphAtNumber(set, editorMultiIndex);
    utf8ReplacePrevious(noteText, editCursor, glyph);
    noteDirty = true;
  } else {
    editorMultiIndex = 0;
    insertEditorGlyph(utf8GlyphAtNumber(set, 0));
  }

  editorLastKey = key; editorLastTime = now; editorMultiActive = true;
  drawEditorBody(); drawEditorFooter();
}

void exitEditorToNotes() {
  if (noteDirty) saveCurrentNote();

  if (editorReturnScreen == SCREEN_PROGRAM_MENU) {
    secureEditorMode = false;
    currentScreen = SCREEN_PROGRAM_MENU;
    drawProgramMenu();
    return;
  }

  if (editorReturnScreen == SCREEN_SECURE_NOTES) {
    refreshSecureNoteList();
    currentScreen = SCREEN_SECURE_NOTES;
    drawSecureNotesScreen();
    return;
  }

  secureEditorMode = false;
  refreshNoteList();
  currentScreen = SCREEN_NOTES;
  drawNotesScreen();
}

void editorKey(char key) {
  if (key == 'A') {
    editorMultiActive = false;
    if (editorMode == INPUT_LAT_LOWER) editorMode = INPUT_LAT_UPPER;
    else if (editorMode == INPUT_LAT_UPPER) editorMode = INPUT_RUS_LOWER;
    else if (editorMode == INPUT_RUS_LOWER) editorMode = INPUT_RUS_UPPER;
    else if (editorMode == INPUT_RUS_UPPER) editorMode = INPUT_NUMBERS;
    else if (editorMode == INPUT_NUMBERS) editorMode = INPUT_NAV;
    else editorMode = INPUT_LAT_LOWER;
    drawEditorHeader(); drawEditorHelp(); drawEditorFooter(); return;
  }

  if (key == 'B') { backspaceEditor(); return; }
  if (key == 'C') { saveCurrentNote(); return; }
  if (key == 'D') {
    exitEditorToNotes();
    return;
  }

  if (editorMode == INPUT_NAV) {
    if (key == '4') moveCursorHorizontal(-1);
    else if (key == '6') moveCursorHorizontal(1);
    else if (key == '2') moveCursorVertical(-1);
    else if (key == '8') moveCursorVertical(1);
    return;
  }

  if (key == '#') {
    editorMultiActive = false; insertEditorGlyph("\n");
    drawEditorBody(); drawEditorFooter(); return;
  }
  editorMultiTap(key);
}




void drawFilesMenu() {
  tft.fillScreen(COLOR_BG); tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("ФАЙЛЫ", "FILES"), COLOR_TEXT);
  for (int i = 0; i < FILES_MENU_ITEMS; i++) {
    int y = 27 + i * 20; uint16_t c = COLOR_GRAY;
    if (i == filesMenuSelected) { tft.fillRoundRect(5, y - 4, 150, 18, 3, COLOR_SELECT); c = COLOR_TEXT; }
    tft.setTextColor(c); tft.setCursor(10, y); tft.print(i + 1); tft.print(".");
    uiText(24, y + 7, filesMenuItemText(i), c);
  }
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(4, 123, T("2/8 ВЫБОР A ОТКР D НАЗАД", "2/8 MOVE A OPEN D BACK"), COLOR_GRAY);
}

void filesMenuKey(char key) {
  if (key == '2') { filesMenuSelected--; if (filesMenuSelected < 0) filesMenuSelected = FILES_MENU_ITEMS - 1; drawFilesMenu(); return; }
  if (key == '8') { filesMenuSelected++; if (filesMenuSelected >= FILES_MENU_ITEMS) filesMenuSelected = 0; drawFilesMenu(); return; }
  if (key == 'A' || key == '#') {
    if (filesMenuSelected == 0) {
      fmCurrentDir = "/"; fmSelected = 0; fmTop = 0;
      refreshFmList();
      currentScreen = SCREEN_FILES; drawFilesScreen();
    } else if (filesMenuSelected == 1) {
      refreshFileList(); fileSelected = 0; fileTop = 0;
      for (int i = 0; i < fileCount; i++) if (isTextFile(fileNames[i])) { fileSelected = i; fileTop = i; break; }
      currentScreen = SCREEN_TEXT_PICKER; drawFilePicker(T("ЧТЕНИЕ ТЕКСТА", "TEXT READER"), true);
    } else if (filesMenuSelected == 2) {
      refreshFileList(); fileSelected = 0; fileTop = 0;
      currentScreen = SCREEN_HEX_PICKER; drawFilePicker(T("HEX-РЕДАКТОР", "HEX EDITOR"), false);
    }
    return;
  }
  if (key == 'D' || key == 'B') { currentScreen = SCREEN_MENU; drawMenu(); }
}

bool isTextFile(const String& path) {
  String p = path; p.toLowerCase();
  return p.endsWith(".txt") || p.endsWith(".log") || p.endsWith(".csv") || p.endsWith(".md") || p.endsWith(".json") || p.endsWith(".ini");
}

void drawFilePicker(const String& title, bool textOnly) {
  tft.fillScreen(COLOR_BG); tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, title, COLOR_TEXT);
  tft.fillRect(0, 20, 160, 87, COLOR_BG);
  int shown = 0;
  for (int i = fileTop; i < fileCount && shown < FILE_VISIBLE_ROWS; i++) {
    if (textOnly && !isTextFile(fileNames[i])) continue;
    int y = 29 + shown * 19; uint16_t c = COLOR_GRAY;
    if (i == fileSelected) { tft.fillRoundRect(2, y - 4, 156, 18, 3, COLOR_SELECT); c = COLOR_TEXT; }
    String n = fileNames[i]; if (n.startsWith("/")) n = n.substring(1);

    drawUtf8(5, y + 8, utf8Truncate(n, 17), c, u8g2_font_6x12_t_cyrillic);
    String st = formatBytes(fileSizes[i]); tft.setTextSize(1); tft.setTextColor(c); tft.setCursor(132, y + 1); tft.print(st);
    shown++;
  }
  if (!shown) uiText(32, 62, textOnly ? T("НЕТ ТЕКСТОВЫХ ФАЙЛОВ", "NO TEXT FILES") : T("НЕТ ФАЙЛОВ", "NO FILES"), COLOR_YELLOW);
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(6, 123, T("2/8 ВЫБОР A ОТКР D НАЗАД", "2/8 MOVE A OPEN D BACK"), COLOR_GRAY);
}

void filePickerKey(char key, bool textOnly) {
  auto valid = [&](int idx) { return idx >= 0 && idx < fileCount && (!textOnly || isTextFile(fileNames[idx])); };
  if (!valid(fileSelected)) {
    bool found = false;
    for (int i = 0; i < fileCount; i++) if (valid(i)) { fileSelected = i; fileTop = i; found = true; break; }
    if (!found) {
      if (key == 'D' || key == 'B') { currentScreen = SCREEN_FILES_MENU; drawFilesMenu(); }
      return;
    }
  }
  if (key == '2') {
    for (int i = fileSelected - 1; i >= 0; i--) if (valid(i)) { fileSelected = i; fileTop = i; break; }
    drawFilePicker(textOnly ? T("ЧТЕНИЕ ТЕКСТА", "TEXT READER") : T("HEX-РЕДАКТОР", "HEX EDITOR"), textOnly); return;
  }
  if (key == '8') {
    for (int i = fileSelected + 1; i < fileCount; i++) if (valid(i)) { fileSelected = i; fileTop = i; break; }
    drawFilePicker(textOnly ? T("ЧТЕНИЕ ТЕКСТА", "TEXT READER") : T("HEX-РЕДАКТОР", "HEX EDITOR"), textOnly); return;
  }
  if (key == 'A' || key == '#') {
    if (textOnly) openTextReader(fileNames[fileSelected]); else openHexViewer(fileNames[fileSelected]);
    return;
  }
  if (key == 'D' || key == 'B') { currentScreen = SCREEN_FILES_MENU; drawFilesMenu(); }
}

int readerTotalLines() {
  int line = 1, col = 0, i = 0;
  while (i < readerText.length()) {
    if (readerText[i] == '\n') { line++; col = 0; i++; continue; }
    i = utf8NextIndex(readerText, i); col++;
    if (col >= READER_COLS) { line++; col = 0; }
  }
  return line;
}

void openTextReader(const String& path) {
  File f = LittleFS.open(path, "r"); if (!f) { showMessage(T("ОШИБКА ОТКРЫТИЯ", "OPEN ERROR"), "", COLOR_RED, 800); return; }
  readerText = f.readString(); f.close(); readerPath = path; readerTopLine = 0;
  currentScreen = SCREEN_TEXT_READER; drawTextReader();
}

void drawTextReader() {
  tft.fillScreen(COLOR_BG); tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  String n = readerPath; if (n.startsWith("/")) n = n.substring(1); n = utf8Truncate(n, 25);
  uiText(3, 13, utf8Truncate(n, 25), COLOR_TEXT);
  int line = 0, col = 0, i = 0;
  while (i < readerText.length()) {
    if (readerText[i] == '\n') { line++; col = 0; i++; continue; }
    String g = utf8GlyphAt(readerText, i);
    if (line >= readerTopLine && line < readerTopLine + READER_ROWS) {
      drawUtf8(4 + col * 6, 31 + (line - readerTopLine) * 12, g, COLOR_TEXT, u8g2_font_6x12_t_cyrillic);
    }
    i = utf8NextIndex(readerText, i); col++;
    if (col >= READER_COLS) { line++; col = 0; }
  }
  int total = readerTotalLines();
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(3, 123, T("2/8 СТРОКА 4/6 СТРАН D НАЗ", "2/8 LINE 4/6 PAGE D BACK"), COLOR_GRAY);
  tft.setTextSize(1); tft.setTextColor(COLOR_GREEN); tft.setCursor(130, 111); tft.print(readerTopLine + 1); tft.print("/"); tft.print(total);
}

void textReaderKey(char key) {
  int maxTop = max(0, readerTotalLines() - READER_ROWS);
  if (key == '2') readerTopLine = max(0, readerTopLine - 1);
  else if (key == '8') readerTopLine = min(maxTop, readerTopLine + 1);
  else if (key == '4') readerTopLine = max(0, readerTopLine - READER_ROWS);
  else if (key == '6') readerTopLine = min(maxTop, readerTopLine + READER_ROWS);
  else if (key == 'D' || key == 'B') { currentScreen = SCREEN_TEXT_PICKER; drawFilePicker(T("ЧТЕНИЕ ТЕКСТА", "TEXT READER"), true); return; }
  else return;
  drawTextReader();
}

void openHexViewer(const String& path) {
  if (!LittleFS.exists(path)) { showMessage(T("ФАЙЛ НЕ НАЙДЕН", "FILE NOT FOUND"), "", COLOR_RED, 800); return; }
  hexPath = path;
  hexOffset = 0;
  hexCursor = 0;
  hexEditMode = false;
  hexAsciiMode = false;
  hexEditDigits = "";
  hexStatus = "";
  currentScreen = SCREEN_HEX_VIEWER;
  drawHexViewer();
}

int hexNibbleValue(char key) {
  if (key >= '0' && key <= '9') return key - '0';
  if (key >= 'A' && key <= 'F') return 10 + (key - 'A');
  if (key == '*') return 14;
  if (key == '#') return 15;
  return -1;
}

bool writeHexByte(size_t offset, uint8_t value) {
  File f = LittleFS.open(hexPath, "r+");
  if (!f) return false;
  if (!f.seek(offset)) { f.close(); return false; }
  size_t n = f.write(&value, 1);
  f.flush();
  f.close();
  return n == 1;
}

void drawHexViewer() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  String n = hexPath;
  if (n.startsWith("/")) n = n.substring(1);
  uiTextSmall(3, 13, utf8Truncate(n, 22), COLOR_TEXT);
  uiTextSmall(132, 13, hexAsciiMode ? "ASC" : "HEX", COLOR_YELLOW);

  File f = LittleFS.open(hexPath, "r");
  if (!f) { uiText(30, 62, T("ОШИБКА ОТКРЫТИЯ", "OPEN ERROR"), COLOR_RED); return; }
  size_t sz = f.size();
  if (sz == 0) {
    f.close();
    uiText(36, 62, T("ПУСТОЙ ФАЙЛ", "EMPTY FILE"), COLOR_YELLOW);
    tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
    uiTextSmall(4, 123, T("D НАЗАД", "D BACK"), COLOR_GRAY);
    return;
  }
  if (hexCursor >= sz) hexCursor = sz - 1;
  size_t pageBytes = HEX_BYTES_PER_ROW * HEX_ROWS;
  if (hexCursor < hexOffset) hexOffset = (hexCursor / HEX_BYTES_PER_ROW) * HEX_BYTES_PER_ROW;
  if (hexCursor >= hexOffset + pageBytes) {
    size_t row = hexCursor / HEX_BYTES_PER_ROW;
    hexOffset = (row >= HEX_ROWS - 1 ? row - (HEX_ROWS - 1) : 0) * HEX_BYTES_PER_ROW;
  }

  tft.setTextSize(1);
  for (int row = 0; row < HEX_ROWS; row++) {
    size_t off = hexOffset + row * HEX_BYTES_PER_ROW;
    if (off >= sz) break;
    int y = 22 + row * 8;
    char addr[8]; snprintf(addr, sizeof(addr), "%04X", (unsigned)(off & 0xFFFF));
    tft.setTextColor(COLOR_GRAY); tft.setCursor(2, y); tft.print(addr);

    char asciiBuf[HEX_BYTES_PER_ROW + 1];
    for (int j = 0; j < HEX_BYTES_PER_ROW; j++) asciiBuf[j] = ' ';
    asciiBuf[HEX_BYTES_PER_ROW] = '\0';

    for (int j = 0; j < HEX_BYTES_PER_ROW; j++) {
      size_t pos = off + j;
      if (pos >= sz) break;
      f.seek(pos);
      uint8_t v = (uint8_t)f.read();
      asciiBuf[j] = (v >= 32 && v <= 126) ? (char)v : '.';

      int x = hexAsciiMode ? (30 + j * 15) : (32 + j * 20);
      char byteText[3]; snprintf(byteText, sizeof(byteText), "%02X", v);
      bool selected = pos == hexCursor;
      if (selected) {
        tft.fillRect(x - 1, y - 1, 14, 8, hexEditMode ? COLOR_RED : COLOR_SELECT);
        tft.setTextColor(COLOR_TEXT);
      } else {
        tft.setTextColor(COLOR_TEXT);
      }
      tft.setCursor(x, y); tft.print(byteText);
    }

    if (hexAsciiMode) {
      tft.setTextColor(COLOR_GREEN, COLOR_BG);
      tft.setCursor(122, y);
      tft.print(asciiBuf);
    }
  }
  f.close();

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  if (hexEditMode) {
    uiTextSmall(2, 119, "HEX: 0-9 A-D *=E #=F", COLOR_YELLOW);
    if (hexEditDigits.length()) {
      tft.setTextSize(1); tft.setTextColor(COLOR_GREEN); tft.setCursor(137, 111); tft.print(hexEditDigits);
    }
  } else if (hexAsciiMode) {
    uiTextSmall(2, 117, T("4/6 БАЙТ 2/8 СТРОКА A РЕД", "4/6 BYTE 2/8 ROW A EDIT"), COLOR_GRAY);
    uiTextSmall(2, 125, T("# HEX-РЕЖИМ D НАЗ", "# HEX MODE D BACK"), COLOR_GRAY);
  } else {
    uiTextSmall(2, 117, T("4/6 БАЙТ 2/8 СТРОКА A РЕД", "4/6 BYTE 2/8 ROW A EDIT"), COLOR_GRAY);
    uiTextSmall(2, 125, T("# HEX+ASCII D НАЗ", "# HEX+ASCII D BACK"), COLOR_GRAY);
  }

  if (hexStatus.length() && millis() < hexStatusUntil) {
    tft.fillRect(88, 109, 72, 9, COLOR_BG);
    uiTextSmall(90, 117, hexStatus, COLOR_GREEN);
  }
}

void hexViewerKey(char key) {
  File f = LittleFS.open(hexPath, "r");
  size_t sz = f ? f.size() : 0;
  if (f) f.close();
  if (sz == 0) {
    if (key == 'D' || key == 'B') { currentScreen = SCREEN_HEX_PICKER; drawFilePicker(T("HEX-РЕДАКТОР", "HEX EDITOR"), false); }
    return;
  }

  if (hexEditMode) {
    int nib = hexNibbleValue(key);
    if (nib < 0) return;
    const char* digits = "0123456789ABCDEF";
    hexEditDigits += digits[nib];
    if (hexEditDigits.length() >= 2) {
      uint8_t value = (uint8_t)((hexNibbleValue(hexEditDigits[0]) << 4) | hexNibbleValue(hexEditDigits[1]));
      if (writeHexByte(hexCursor, value)) {
        hexStatus = T("ЗАПИСАНО", "WRITTEN");
        hexStatusUntil = millis() + 900;
        hexEditMode = false;
      } else {
        hexStatus = T("ОШИБКА", "ERROR");
        hexStatusUntil = millis() + 1200;
      }
      hexEditDigits = "";
    }
    drawHexViewer();
    return;
  }

  if (key == '4') { if (hexCursor > 0) hexCursor--; }
  else if (key == '6') { if (hexCursor + 1 < sz) hexCursor++; }
  else if (key == '2') { hexCursor = hexCursor >= HEX_BYTES_PER_ROW ? hexCursor - HEX_BYTES_PER_ROW : 0; }
  else if (key == '8') { if (hexCursor + HEX_BYTES_PER_ROW < sz) hexCursor += HEX_BYTES_PER_ROW; }
  else if (key == 'A') { hexEditMode = true; hexEditDigits = ""; }
  else if (key == '#') { hexAsciiMode = !hexAsciiMode; }
  else if (key == 'D' || key == 'B') { currentScreen = SCREEN_HEX_PICKER; drawFilePicker(T("HEX-РЕДАКТОР", "HEX EDITOR"), false); return; }
  else return;
  drawHexViewer();
}






bool isImageFile(const String& path) {
  String p = path; p.toLowerCase();
  return p.endsWith(".bmp");
}

uint16_t read16le(File& f) {
  uint16_t v = (uint8_t)f.read();
  v |= ((uint16_t)(uint8_t)f.read()) << 8;
  return v;
}

uint32_t read32le(File& f) {
  uint32_t v = (uint8_t)f.read();
  v |= ((uint32_t)(uint8_t)f.read()) << 8;
  v |= ((uint32_t)(uint8_t)f.read()) << 16;
  v |= ((uint32_t)(uint8_t)f.read()) << 24;
  return v;
}

bool loadBmpInfo(File& f) {
  if (!f) return false;
  f.seek(0);
  if (read16le(f) != 0x4D42) return false;
  (void)read32le(f);
  (void)read32le(f);
  imageDataOffset = read32le(f);
  uint32_t dibSize = read32le(f);
  if (dibSize < 40) return false;
  imageWidth = (int32_t)read32le(f);
  int32_t h = (int32_t)read32le(f);
  imageTopDown = h < 0;
  imageHeight = h < 0 ? -h : h;
  uint16_t planes = read16le(f);
  imageBitDepth = read16le(f);
  imageCompression = read32le(f);
  if (planes != 1 || imageWidth <= 0 || imageHeight <= 0) return false;
  if (imageCompression != 0) return false;
  return imageBitDepth == 24 || imageBitDepth == 32;
}

void drawImagePicker() {
  tft.fillScreen(COLOR_BG); tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ИЗОБРАЖЕНИЯ", "IMAGES"), COLOR_TEXT);
  int shown = 0;
  for (int i = fileTop; i < fileCount && shown < FILE_VISIBLE_ROWS; i++) {
    if (!isImageFile(fileNames[i])) continue;
    int y = 29 + shown * 19; uint16_t c = COLOR_GRAY;
    if (i == fileSelected) { tft.fillRoundRect(2, y - 4, 156, 18, 3, COLOR_SELECT); c = COLOR_TEXT; }
    String n = fileNames[i]; if (n.startsWith("/")) n = n.substring(1);
    drawUtf8(5, y + 8, utf8Truncate(n, 19), c, u8g2_font_6x12_t_cyrillic);
    shown++;
  }
  if (!shown) uiText(18, 62, T("НЕТ BMP-ФАЙЛОВ", "NO BMP FILES"), COLOR_YELLOW);
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(5, 123, T("2/8 ВЫБОР A ОТКР D НАЗАД", "2/8 MOVE A OPEN D BACK"), COLOR_GRAY);
}

void imagePickerKey(char key) {
  auto valid = [&](int idx) { return idx >= 0 && idx < fileCount && isImageFile(fileNames[idx]); };
  if (!valid(fileSelected)) {
    bool found = false;
    for (int i = 0; i < fileCount; i++) if (valid(i)) { fileSelected = i; fileTop = i; found = true; break; }
    if (!found) { if (key == 'D' || key == 'B') { currentScreen = SCREEN_FILES_MENU; drawFilesMenu(); } return; }
  }
  if (key == '2') {
    for (int i = fileSelected - 1; i >= 0; i--) if (valid(i)) { fileSelected = i; fileTop = i; break; }
    drawImagePicker(); return;
  }
  if (key == '8') {
    for (int i = fileSelected + 1; i < fileCount; i++) if (valid(i)) { fileSelected = i; fileTop = i; break; }
    drawImagePicker(); return;
  }
  if (key == 'A' || key == '#') { openImageViewer(fileNames[fileSelected]); return; }
  if (key == 'D' || key == 'B') { currentScreen = SCREEN_FILES_MENU; drawFilesMenu(); }
}

void openImageViewer(const String& path) {
  File f = LittleFS.open(path, "r");
  if (!f || !loadBmpInfo(f)) {
    if (f) f.close();
    showMessage(T("BMP НЕ ПОДДЕРЖИВАЕТСЯ", "BMP NOT SUPPORTED"), T("НУЖЕН 24/32 BIT", "USE 24/32 BIT"), COLOR_RED, 1200);
    return;
  }
  f.close();
  imagePath = path;
  imagePanX = 0;
  imagePanY = 0;
  imageDeleteArmed = false;
  currentScreen = SCREEN_IMAGE_VIEWER;
  drawImageViewer();
}

void drawImageViewer() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  String n = imagePath; if (n.startsWith("/")) n = n.substring(1);
  uiTextSmall(3, 13, utf8Truncate(n, 26), COLOR_TEXT);

  File f = LittleFS.open(imagePath, "r");
  if (!f || !loadBmpInfo(f)) {
    if (f) f.close();
    uiText(20, 62, T("ОШИБКА ИЗОБРАЖЕНИЯ", "IMAGE ERROR"), COLOR_RED);
    return;
  }

  int maxPanX = max(0, (int)imageWidth - 160);
  int maxPanY = max(0, (int)imageHeight - IMAGE_VIEW_H);
  imagePanX = constrain(imagePanX, 0, maxPanX);
  imagePanY = constrain(imagePanY, 0, maxPanY);

  int drawW = min(160, (int)imageWidth);
  int drawH = min(IMAGE_VIEW_H, (int)imageHeight);
  int destX = imageWidth < 160 ? (160 - imageWidth) / 2 : 0;
  int destY = IMAGE_VIEW_Y + (imageHeight < IMAGE_VIEW_H ? (IMAGE_VIEW_H - imageHeight) / 2 : 0);
  int bytesPerPixel = imageBitDepth / 8;
  uint32_t rowSize = ((uint32_t)imageWidth * bytesPerPixel + 3) & ~3UL;

  static uint8_t raw[160 * 4];
  static uint16_t line565[160];

  for (int dy = 0; dy < drawH; dy++) {
    int srcYTop = imagePanY + dy;
    int srcRow = imageTopDown ? srcYTop : (imageHeight - 1 - srcYTop);
    uint32_t pos = imageDataOffset + (uint32_t)srcRow * rowSize + (uint32_t)imagePanX * bytesPerPixel;
    if (!f.seek(pos)) break;
    size_t need = drawW * bytesPerPixel;
    size_t got = f.read(raw, need);
    if (got < need) break;
    for (int x = 0; x < drawW; x++) {
      uint8_t b = raw[x * bytesPerPixel + 0];
      uint8_t g = raw[x * bytesPerPixel + 1];
      uint8_t r = raw[x * bytesPerPixel + 2];
      line565[x] = tft.color565(r, g, b);
    }
    tft.drawRGBBitmap(destX, destY + dy, line565, drawW, 1);
  }
  f.close();

  tft.fillRect(0, 114, 160, 14, COLOR_BG);
  tft.drawFastHLine(0, 114, 160, COLOR_GRAY);
  if (imageDeleteArmed) {
    uiTextSmall(2, 126, T("* ЕЩЁ РАЗ = УДАЛИТЬ", "* AGAIN = DELETE"), COLOR_RED);
  } else {
    uiTextSmall(2, 126, T("A/C ФОТО # ОБОИ * УДАЛ D НАЗ", "A/C PHOTO # WALL * DEL D BACK"), COLOR_GRAY);
  }
}

int findImageFileIndex(const String& path) {
  for (int i = 0; i < fmCount; i++) {
    if (!fmIsDir[i] && fmPaths[i] == path) return i;
  }
  return -1;
}

bool openAdjacentImage(int direction) {


  if (fmCount <= 0) return false;
  int current = findImageFileIndex(imagePath);
  if (current < 0) current = fmSelected;

  for (int step = 1; step <= fmCount; step++) {
    int idx = current + direction * step;
    while (idx < 0) idx += fmCount;
    while (idx >= fmCount) idx -= fmCount;
    if (fmIsDir[idx] || !isImageFile(fmPaths[idx])) continue;

    File f = LittleFS.open(fmPaths[idx], "r");
    bool ok = f && loadBmpInfo(f);
    if (f) f.close();
    if (!ok) continue;

    fmSelected = idx;
    if (fmSelected < fmTop) fmTop = fmSelected;
    if (fmSelected >= fmTop + FILE_VISIBLE_ROWS) fmTop = fmSelected - FILE_VISIBLE_ROWS + 1;
    imagePath = fmPaths[idx];
    imagePanX = 0;
    imagePanY = 0;
    imageDeleteArmed = false;
    drawImageViewer();
    return true;
  }
  return false;
}

bool deleteCurrentImage() {
  if (imagePath.length() == 0 || !LittleFS.exists(imagePath)) return false;

  int oldIndex = findImageFileIndex(imagePath);
  String deletedPath = imagePath;
  if (!LittleFS.remove(deletedPath)) return false;

  if (deletedPath == wallpaperPath) {
    wallpaperPath = "";
    preferences.putString("wallpaper", "");
  }

  refreshFileList();
  refreshFmList();


  if (fmCount > 0) {
    int startIndex = oldIndex;
    if (startIndex < 0) startIndex = 0;
    if (startIndex >= fmCount) startIndex = fmCount - 1;

    for (int pass = 0; pass < 2; pass++) {
      int begin = pass == 0 ? startIndex : 0;
      int finish = pass == 0 ? fmCount : startIndex;
      for (int i = begin; i < finish; i++) {
        if (fmIsDir[i] || !isImageFile(fmPaths[i])) continue;
        File f = LittleFS.open(fmPaths[i], "r");
        bool ok = f && loadBmpInfo(f);
        if (f) f.close();
        if (!ok) continue;
        fmSelected = i;
        imagePath = fmPaths[i];
        imagePanX = 0;
        imagePanY = 0;
        imageDeleteArmed = false;
        drawImageViewer();
        return true;
      }
    }
  }


  imagePath = "";
  imageDeleteArmed = false;
  currentScreen = SCREEN_FILES;
  drawFilesScreen();
  return true;
}

void imageViewerKey(char key) {
  const int stepX = 16;
  const int stepY = 12;
  int maxPanX = max(0, (int)imageWidth - 160);
  int maxPanY = max(0, (int)imageHeight - IMAGE_VIEW_H);

  if (key != '*' && imageDeleteArmed) imageDeleteArmed = false;

  if (key == '4') imagePanX = max(0, imagePanX - stepX);
  else if (key == '6') imagePanX = min(maxPanX, imagePanX + stepX);
  else if (key == '2') imagePanY = max(0, imagePanY - stepY);
  else if (key == '8') imagePanY = min(maxPanY, imagePanY + stepY);
  else if (key == 'A') { openAdjacentImage(+1); return; }
  else if (key == 'C') { openAdjacentImage(-1); return; }
  else if (key == '#') {
    applyWallpaper(imagePath);
    tft.fillRect(0, 114, 160, 14, COLOR_BG);
    tft.drawFastHLine(0, 114, 160, COLOR_GRAY);
    uiTextSmall(5, 126, T("УСТАНОВЛЕНО КАК ОБОИ", "SET AS WALLPAPER"), COLOR_GREEN);
    delay(650);
    drawImageViewer();
    return;
  }
  else if (key == '*') {
    if (!imageDeleteArmed) {
      imageDeleteArmed = true;
      drawImageViewer();
      return;
    }
    deleteCurrentImage();
    return;
  }
  else if (key == 'D' || key == 'B') {
    imageDeleteArmed = false;
    refreshFmList();
    currentScreen = SCREEN_FILES;
    drawFilesScreen();
    return;
  }
  else return;

  drawImageViewer();
}




String formatBytes(size_t bytes) {
  if (bytes < 1024) return String(bytes) + "B";
  if (bytes < 1024UL * 1024UL) return String(bytes / 1024.0, 1) + "K";
  return String(bytes / (1024.0 * 1024.0), 1) + "M";
}




void refreshFileList() {
  fileCount = 0;
  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) return;
  File file = root.openNextFile();
  while (file && fileCount < MAX_FILES) {
    if (!file.isDirectory()) {
      String name = String(file.name());
      if (!name.startsWith("/")) name = "/" + name;
      fileNames[fileCount] = name;
      fileSizes[fileCount] = file.size();
      fileCount++;
    }
    file.close(); file = root.openNextFile();
  }
  root.close();
  if (fileSelected >= fileCount) fileSelected = fileCount - 1;
  if (fileSelected < 0) fileSelected = 0;
  if (fileTop > fileSelected) fileTop = fileSelected;
  fileDeleteArmed = false;
}

String fmBaseName(const String& path) {
  if (path == "/") return "/";
  int slash = path.lastIndexOf('/');
  if (slash < 0) return path;
  return path.substring(slash + 1);
}

String fmParentDir(const String& path) {
  if (path.length() == 0 || path == "/") return "/";
  String p = path;
  while (p.length() > 1 && p.endsWith("/")) p.remove(p.length() - 1);
  int slash = p.lastIndexOf('/');
  if (slash <= 0) return "/";
  return p.substring(0, slash);
}

String fmJoinPath(const String& dir, const String& name) {
  String d = dir.length() ? dir : "/";
  if (!d.startsWith("/")) d = "/" + d;
  while (d.length() > 1 && d.endsWith("/")) d.remove(d.length() - 1);
  String n = name;
  while (n.startsWith("/")) n.remove(0, 1);
  return d == "/" ? "/" + n : d + "/" + n;
}

String fmExtension(const String& path) {
  String n = fmBaseName(path);
  int dot = n.lastIndexOf('.');
  if (dot <= 0 || dot >= n.length() - 1) return T("НЕТ", "NONE");
  String e = n.substring(dot + 1); e.toUpperCase();
  return e;
}

bool fmProtectedPath(const String& path) {



  return path == "/events" || path == "/secure" || path == "/contacts";
}

void refreshFmList() {
  fmCount = 0;
  File dir = LittleFS.open(fmCurrentDir, "r");
  if (!dir || !dir.isDirectory()) {
    if (dir) dir.close();
    fmCurrentDir = "/";
    dir = LittleFS.open("/", "r");
  }
  if (!dir) return;

  File item = dir.openNextFile();
  while (item && fmCount < FM_MAX_ITEMS) {
    String full = String(item.name());
    if (!full.startsWith("/")) full = fmJoinPath(fmCurrentDir, full);
    fmPaths[fmCount] = full;
    fmNames[fmCount] = fmBaseName(full);
    fmIsDir[fmCount] = item.isDirectory();
    fmSizes[fmCount] = item.isDirectory() ? 0 : item.size();
    fmCount++;
    item.close();
    item = dir.openNextFile();
  }
  dir.close();



  for (int i = 0; i < fmCount - 1; i++) {
    for (int j = i + 1; j < fmCount; j++) {
      String ai = fmNames[i]; ai.toLowerCase();
      String aj = fmNames[j]; aj.toLowerCase();
      bool swapNeeded = (!fmIsDir[i] && fmIsDir[j]) ||
                        (fmIsDir[i] == fmIsDir[j] && ai.compareTo(aj) > 0);
      if (swapNeeded) {
        String tp = fmPaths[i]; fmPaths[i] = fmPaths[j]; fmPaths[j] = tp;
        String tn = fmNames[i]; fmNames[i] = fmNames[j]; fmNames[j] = tn;
        size_t ts = fmSizes[i]; fmSizes[i] = fmSizes[j]; fmSizes[j] = ts;
        bool td = fmIsDir[i]; fmIsDir[i] = fmIsDir[j]; fmIsDir[j] = td;
      }
    }
  }

  if (fmSelected >= fmCount) fmSelected = fmCount - 1;
  if (fmSelected < 0) fmSelected = 0;
  if (fmTop > fmSelected) fmTop = fmSelected;
  if (fmTop < 0) fmTop = 0;
  fmDeleteArmed = false;
}

void drawFilesScreen() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("МЕНЕДЖЕР ФАЙЛОВ", "FILE MANAGER"), COLOR_TEXT);

  String shownPath = fmCurrentDir;
  drawUtf8(3, 27, utf8Truncate(shownPath, 30), COLOR_GREEN, u8g2_font_5x7_t_cyrillic);
  drawFileRows();

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(2, 116, T("A ОТКР # ОПЕР C УДАЛ", "A OPEN # OPS C DELETE"), COLOR_GRAY);
  uiTextSmall(2, 126, T("2/8 ВЫБ B ВВЕРХ D НАЗ", "2/8 SEL B UP D BACK"), COLOR_GRAY);
}

void drawFileRows() {
  tft.fillRect(0, 29, 160, 78, COLOR_BG);
  if (fmCount <= 0) {
    uiText(42, 63, T("ПАПКА ПУСТА", "FOLDER EMPTY"), COLOR_YELLOW);
    return;
  }

  if (fmSelected < fmTop) fmTop = fmSelected;
  if (fmSelected >= fmTop + FILE_VISIBLE_ROWS) fmTop = fmSelected - FILE_VISIBLE_ROWS + 1;

  for (int row = 0; row < FILE_VISIBLE_ROWS; row++) {
    int index = fmTop + row;
    if (index >= fmCount) break;
    int y = 33 + row * 18;
    uint16_t c = COLOR_GRAY;
    if (index == fmSelected) {
      tft.fillRoundRect(2, y - 2, 156, 16, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }

    if (fmIsDir[index]) {


      uiTextSmall(5, y + 8, "[D]", COLOR_YELLOW);
      drawUtf8(25, y + 11, utf8Truncate(fmNames[index], 20), c, u8g2_font_6x12_t_cyrillic);
    } else {



      drawUtf8(5, y + 11, utf8Truncate(fmNames[index], 17), c, u8g2_font_6x12_t_cyrillic);
      String st = formatBytes(fmSizes[index]);
      tft.setTextSize(1); tft.setTextColor(c, COLOR_BG);
      tft.setCursor(157 - st.length() * 6, y + 3); tft.print(st);
    }
  }
}

bool fmDeleteRecursive(const String& path) {
  File item = LittleFS.open(path, "r");
  if (!item) return false;
  bool isDir = item.isDirectory();
  item.close();

  if (!isDir) return LittleFS.remove(path);



  while (true) {
    File dir = LittleFS.open(path, "r");
    if (!dir) return false;
    File child = dir.openNextFile();
    if (!child) { dir.close(); break; }
    String childPath = String(child.name());
    if (!childPath.startsWith("/")) childPath = fmJoinPath(path, childPath);
    child.close();
    dir.close();
    if (!fmDeleteRecursive(childPath)) return false;
  }
  return LittleFS.rmdir(path);
}

void fmUpdateWallpaperPath(const String& oldPath, const String& newPath) {
  if (wallpaperPath == oldPath) {
    wallpaperPath = newPath;
    preferences.putString("wallpaper", wallpaperPath);
    return;
  }
  String prefix = oldPath;
  if (!prefix.endsWith("/")) prefix += "/";
  if (wallpaperPath.startsWith(prefix)) {
    String suffix = wallpaperPath.substring(oldPath.length());
    wallpaperPath = newPath.length() ? newPath + suffix : "";
    preferences.putString("wallpaper", wallpaperPath);
  }
}

void deleteSelectedFile() {
  if (fmCount <= 0) return;
  String path = fmPaths[fmSelected];
  if (fmProtectedPath(path)) {
    showMessage(T("СИСТЕМНАЯ ПАПКА", "SYSTEM FOLDER"), T("УДАЛЕНИЕ ЗАПРЕЩЕНО", "DELETE BLOCKED"), COLOR_RED, 850);
    currentScreen = SCREEN_FILES; drawFilesScreen();
    return;
  }

  bool ok = fmDeleteRecursive(path);
  if (ok) {
    fmUpdateWallpaperPath(path, "");
    refreshFileList();
    refreshFmList();
    showMessage(T("УДАЛЕНО", "DELETED"), "", COLOR_GREEN, 350);
  } else {
    showMessage(T("ОШИБКА УДАЛЕНИЯ", "DELETE ERROR"), "", COLOR_RED, 650);
  }
  currentScreen = SCREEN_FILES;
  drawFilesScreen();
}

void openSelectedFile() {
  if (fmCount <= 0) return;
  String path = fmPaths[fmSelected];
  if (fmIsDir[fmSelected]) {
    fmCurrentDir = path;
    fmSelected = 0; fmTop = 0;
    refreshFmList();
    drawFilesScreen();
    return;
  }



  if (isTextFile(path)) { openNote(path); return; }
  if (isImageFile(path)) { openImageViewer(path); return; }
  showMessage(T("ИСПОЛЬЗУЙТЕ HEX", "USE HEX EDITOR"), T("ДЛЯ БИНАРНОГО ФАЙЛА", "FOR BINARY FILE"), COLOR_YELLOW, 700);
  currentScreen = SCREEN_FILES; drawFilesScreen();
}

void filesKey(char key) {
  if (key != 'C') fmDeleteArmed = false;

  if (key == '2' && fmSelected > 0) {
    fmSelected--;
    if (fmSelected < fmTop) fmTop = fmSelected;
    drawFileRows(); return;
  }
  if (key == '8' && fmSelected < fmCount - 1) {
    fmSelected++;
    if (fmSelected >= fmTop + FILE_VISIBLE_ROWS) fmTop = fmSelected - FILE_VISIBLE_ROWS + 1;
    drawFileRows(); return;
  }
  if (key == 'A') { openSelectedFile(); return; }

  if (key == '#') {
    fileOpsDeleteArmed = false;
    fileOpsSelected = fmCount > 0 ? 0 : 5;
    currentScreen = SCREEN_FILE_OPERATIONS;
    drawFileOperations();
    return;
  }

  if (key == 'C') {
    if (fmCount <= 0) return;
    if (!fmDeleteArmed) {
      fmDeleteArmed = true;
      tft.fillRect(0, 108, 160, 20, COLOR_BG);
      uiTextSmall(5, 121, T("C ЕЩЁ РАЗ = УДАЛИТЬ", "PRESS C AGAIN = DELETE"), COLOR_RED);
    } else {
      fmDeleteArmed = false;
      deleteSelectedFile();
    }
    return;
  }

  if (key == 'B') {
    if (fmCurrentDir != "/") {
      fmCurrentDir = fmParentDir(fmCurrentDir);
      fmSelected = 0; fmTop = 0;
      refreshFmList(); drawFilesScreen();
    }
    return;
  }

  if (key == 'D') {
    currentScreen = SCREEN_FILES_MENU;
    drawFilesMenu();
  }
}


void drawFileOperations() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ОПЕРАЦИИ", "FILE OPERATIONS"), COLOR_TEXT);

  String target = fmCount > 0 ? fmNames[fmSelected] : T("НЕТ ФАЙЛА", "NO FILE");
  drawUtf8(4, 27, utf8Truncate(target, 28), COLOR_GREEN, u8g2_font_5x7_t_cyrillic);

  const char* ru[FILE_OP_ITEMS] = {"ИНФОРМАЦИЯ", "ПЕРЕИМЕНОВАТЬ", "КОПИРОВАТЬ", "ПЕРЕМЕСТИТЬ", "УДАЛИТЬ", "НОВАЯ ПАПКА"};
  const char* en[FILE_OP_ITEMS] = {"INFORMATION", "RENAME", "COPY", "MOVE", "DELETE", "NEW FOLDER"};
  for (int i = 0; i < FILE_OP_ITEMS; i++) {
    int y = 31 + i * 12;
    uint16_t c = COLOR_GRAY;
    if (i == fileOpsSelected) {
      tft.fillRoundRect(3, y - 2, 154, 12, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    tft.setTextSize(1); tft.setTextColor(c, COLOR_BG); tft.setCursor(7, y); tft.print(i + 1); tft.print('.');
    drawUtf8(23, y + 7, systemLanguage == LANG_RU ? String(ru[i]) : String(en[i]), c, u8g2_font_5x7_t_cyrillic);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  if (fileOpsDeleteArmed && fileOpsSelected == 4)
    uiTextSmall(4, 123, T("A/# ЕЩЁ РАЗ = УДАЛИТЬ", "A/# AGAIN = DELETE"), COLOR_RED);
  else
    uiTextSmall(4, 123, T("2/8 ВЫБОР A ОК D НАЗ", "2/8 MOVE A OK D BACK"), COLOR_GRAY);
}

void fileOperationsKey(char key) {
  if (key == '2') {
    fileOpsDeleteArmed = false;
    fileOpsSelected--;
    if (fileOpsSelected < 0) fileOpsSelected = FILE_OP_ITEMS - 1;
    drawFileOperations(); return;
  }
  if (key == '8') {
    fileOpsDeleteArmed = false;
    fileOpsSelected++;
    if (fileOpsSelected >= FILE_OP_ITEMS) fileOpsSelected = 0;
    drawFileOperations(); return;
  }
  if (key == 'D' || key == 'B') {
    fileOpsDeleteArmed = false;
    currentScreen = SCREEN_FILES; drawFilesScreen(); return;
  }
  if (key != 'A' && key != '#') return;

  if (fileOpsSelected == 5) {
    fileOpsDeleteArmed = false;
    beginFileNameInput(FNAME_NEW_FOLDER);
    return;
  }

  if (fmCount <= 0) {
    showMessage(T("НЕТ ВЫБРАННОГО ФАЙЛА", "NO SELECTED ITEM"), "", COLOR_RED, 600);
    currentScreen = SCREEN_FILE_OPERATIONS; drawFileOperations();
    return;
  }

  fileOpSourcePath = fmPaths[fmSelected];
  if ((fileOpsSelected == 1 || fileOpsSelected == 3 || fileOpsSelected == 4) && fmProtectedPath(fileOpSourcePath)) {
    showMessage(T("СИСТЕМНАЯ ПАПКА", "SYSTEM FOLDER"), T("ОПЕРАЦИЯ ЗАПРЕЩЕНА", "OPERATION BLOCKED"), COLOR_RED, 800);
    currentScreen = SCREEN_FILE_OPERATIONS; drawFileOperations();
    return;
  }

  if (fileOpsSelected == 0) {
    currentScreen = SCREEN_FILE_INFO;
    drawFileInfo();
  } else if (fileOpsSelected == 1) {
    beginFileNameInput(FNAME_RENAME);
  } else if (fileOpsSelected == 2 || fileOpsSelected == 3) {
    fileOpsDeleteArmed = false;
    folderPickerMove = (fileOpsSelected == 3);
    folderPickerDir = "/";
    folderSelected = 0; folderTop = 0;
    refreshFolderPicker();
    currentScreen = SCREEN_FOLDER_PICKER;
    drawFolderPicker();
  } else if (fileOpsSelected == 4) {
    if (!fileOpsDeleteArmed) {
      fileOpsDeleteArmed = true;
      drawFileOperations();
      return;
    }
    fileOpsDeleteArmed = false;
    deleteSelectedFile();
  }
}

bool validFileName(String name) {
  name.trim();
  if (!name.length() || name == "." || name == "..") return false;
  if (name.length() > MAX_FILE_NAME_BYTES) return false;
  for (int i = 0; i < name.length(); i++) {
    uint8_t c = (uint8_t)name[i];
    if (c < 32 || name[i] == '/' || name[i] == '\\') return false;
  }
  return true;
}

void beginFileNameInput(FileNameAction action) {
  fileNameAction = action;
  fileNameInputMode = systemLanguage == LANG_RU ? INPUT_RUS_LOWER : INPUT_LAT_LOWER;
  fileNameMultiActive = false;
  if (action == FNAME_RENAME && fmCount > 0) fileNameInput = fmNames[fmSelected];
  else fileNameInput = "";
  fileNameCursor = fileNameInput.length();
  currentScreen = SCREEN_FILE_NAME_INPUT;
  drawFileNameInput();
}

void drawFileNameInput() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, fileNameAction == FNAME_RENAME ? T("ПЕРЕИМЕНОВАНИЕ", "RENAME") : T("НОВАЯ ПАПКА", "NEW FOLDER"), COLOR_TEXT);

  uiTextSmall(5, 30, T("ИМЯ:", "NAME:"), COLOR_GRAY);
  tft.drawRoundRect(4, 35, 152, 24, 3, COLOR_GRAY);
  tft.fillRect(7, 38, 146, 18, COLOR_BG);
  int fnCursorGlyph = sharedCursorGlyphIndex(fileNameInput, fileNameCursor);
  int fnStartGlyph = fnCursorGlyph > 20 ? fnCursorGlyph - 20 : 0;
  int fnStartByte = sharedCursorByteForGlyph(fileNameInput, fnStartGlyph);
  String fnShown = utf8Truncate(fileNameInput.substring(fnStartByte), 22);
  if (!fnShown.length() && !fileNameInput.length()) fnShown = "_";
  drawUtf8(9, 52, fnShown, COLOR_TEXT, u8g2_font_6x12_t_cyrillic);
  int fnCol = fnCursorGlyph - fnStartGlyph; if (fnCol < 0) fnCol = 0; if (fnCol > 22) fnCol = 22;
  tft.drawFastVLine(9 + fnCol * 6, 40, 12, COLOR_ACCENT);

  uiTextSmall(5, 68, T("РЕЖИМ", "MODE"), COLOR_GRAY);
  uiTextSmall(45, 68, inputModeName(fileNameInputMode), COLOR_YELLOW);



  const uint8_t* hf = u8g2_font_4x6_t_cyrillic;
  if (fileNameInputMode == INPUT_RUS_LOWER || fileNameInputMode == INPUT_RUS_UPPER) {
    bool up = fileNameInputMode == INPUT_RUS_UPPER;
    drawUtf8(4, 78, up ? "2 АБВГ  3 ДЕЁЖЗ" : "2 абвг  3 деёжз", COLOR_GRAY, hf);
    drawUtf8(4, 86, up ? "4 ИЙКЛ  5 МНОП" : "4 ийкл  5 мноп", COLOR_GRAY, hf);
    drawUtf8(4, 94, up ? "6 РСТУ 7 ФХЦЧ" : "6 рсту 7 фхцч", COLOR_GRAY, hf);
    drawUtf8(4, 102, up ? "8 ШЩЪЫ 9 ЬЭЮЯ 0 ПРОБ" : "8 шщъы 9 ьэюя 0 проб", COLOR_GRAY, hf);
  } else if (fileNameInputMode == INPUT_NAV) {
    drawUtf8(4, 80, T("NAV: 4 <   6 >", "NAV: 4 <   6 >"), COLOR_GRAY, hf);
    drawUtf8(4, 90, T("2 НАЧАЛО 8 КОНЕЦ", "2 HOME 8 END"), COLOR_GRAY, hf);
    drawUtf8(4, 100, T("A СМЕНА РЕЖИМА", "A CHANGE MODE"), COLOR_GRAY, hf);
  } else if (fileNameInputMode == INPUT_NUMBERS) {
    drawUtf8(4, 82, T("0-9 ЦИФРЫ", "0-9 DIGITS"), COLOR_GRAY, hf);
    drawUtf8(4, 92, T("* @ . _ - !", "* @ . _ - !"), COLOR_GRAY, hf);
    drawUtf8(4, 102, T("A СМЕНА РЕЖИМА", "A CHANGE MODE"), COLOR_GRAY, hf);
  } else {
    bool up = fileNameInputMode == INPUT_LAT_UPPER;
    drawUtf8(4, 78, up ? "2 ABC 3 DEF 4 GHI" : "2 abc 3 def 4 ghi", COLOR_GRAY, hf);
    drawUtf8(4, 86, up ? "5 JKL 6 MNO 7 PQRS" : "5 jkl 6 mno 7 pqrs", COLOR_GRAY, hf);
    drawUtf8(4, 94, up ? "8 TUV 9 WXYZ" : "8 tuv 9 wxyz", COLOR_GRAY, hf);
    drawUtf8(4, 102, T("0 ПРОБЕЛ  * СИМВОЛЫ", "0 SPACE  * SYMBOLS"), COLOR_GRAY, hf);
  }
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(2, 116, T("A РЕЖ B УДАЛ C ГОТОВО", "A MODE B DEL C DONE"), COLOR_GRAY);
  uiTextSmall(2, 126, T("D ОТМЕНА", "D CANCEL"), COLOR_GRAY);
}

void fileNameMultiTap(char key) {
  if (fileNameInputMode == INPUT_NAV) return;
  if (fileNameInputMode == INPUT_NUMBERS && key >= '0' && key <= '9') {
    sharedInsert(fileNameInput, fileNameCursor, String(key), MAX_FILE_NAME_BYTES);
    fileNameMultiActive = false; drawFileNameInput(); return;
  }
  String set = getCharacterSet(key, fileNameInputMode);
  if (!set.length()) return;
  unsigned long now = millis();
  bool same = fileNameMultiActive && key == fileNameLastKey && now - fileNameLastTime < 900;
  int glyphCount = utf8Count(set);
  if (same && fileNameCursor > 0) {
    fileNameMultiIndex = (fileNameMultiIndex + 1) % max(1, glyphCount);
    utf8ReplacePrevious(fileNameInput, fileNameCursor, utf8GlyphAtNumber(set, fileNameMultiIndex));
  } else {
    fileNameMultiIndex = 0; sharedInsert(fileNameInput, fileNameCursor, utf8GlyphAtNumber(set, 0), MAX_FILE_NAME_BYTES);
  }
  fileNameLastKey = key; fileNameLastTime = now; fileNameMultiActive = true; drawFileNameInput();
}

void fileNameInputKey(char key) {
  if (key == 'A') {
    fileNameMultiActive = false;
    if (fileNameInputMode == INPUT_LAT_LOWER) fileNameInputMode = INPUT_LAT_UPPER;
    else if (fileNameInputMode == INPUT_LAT_UPPER) fileNameInputMode = INPUT_RUS_LOWER;
    else if (fileNameInputMode == INPUT_RUS_LOWER) fileNameInputMode = INPUT_RUS_UPPER;
    else if (fileNameInputMode == INPUT_RUS_UPPER) fileNameInputMode = INPUT_NUMBERS;
    else if (fileNameInputMode == INPUT_NUMBERS) fileNameInputMode = INPUT_NAV;
    else fileNameInputMode = INPUT_LAT_LOWER;
    drawFileNameInput(); return;
  }

  if (fileNameInputMode == INPUT_NAV && (key == '2' || key == '8' || key == '4' || key == '6')) {
    fileNameMultiActive = false;
    if (key == '4') sharedCursorMoveHorizontal(fileNameInput, fileNameCursor, -1);
    else if (key == '6') sharedCursorMoveHorizontal(fileNameInput, fileNameCursor, 1);
    else if (key == '2') fileNameCursor = 0;
    else if (key == '8') fileNameCursor = fileNameInput.length();
    drawFileNameInput(); return;
  }

  if (key == 'B') {
    fileNameMultiActive = false;
    sharedBackspace(fileNameInput, fileNameCursor);
    drawFileNameInput(); return;
  }

  if (key == 'D') {
    fileNameMultiActive = false;
    currentScreen = SCREEN_FILE_OPERATIONS;
    drawFileOperations();
    return;
  }

  if (key == 'C' || key == '#') {
    fileNameMultiActive = false;
    String clean = fileNameInput; clean.trim();
    if (!validFileName(clean)) {
      showMessage(T("НЕДОПУСТИМОЕ ИМЯ", "INVALID NAME"), T("БЕЗ СЛЭШЕЙ", "NO SLASHES"), COLOR_RED, 750);
      currentScreen = SCREEN_FILE_NAME_INPUT; drawFileNameInput();
      return;
    }

    if (fileNameAction == FNAME_NEW_FOLDER) {
      String path = fmJoinPath(fmCurrentDir, clean);
      if (LittleFS.exists(path)) {
        showMessage(T("ИМЯ УЖЕ ЗАНЯТО", "NAME EXISTS"), "", COLOR_RED, 650);
        currentScreen = SCREEN_FILE_NAME_INPUT; drawFileNameInput(); return;
      }
      bool ok = LittleFS.mkdir(path);
      showMessage(ok ? T("ПАПКА СОЗДАНА", "FOLDER CREATED") : T("ОШИБКА", "ERROR"), "", ok ? COLOR_GREEN : COLOR_RED, 450);
      refreshFmList();
      currentScreen = SCREEN_FILES; drawFilesScreen();
      return;
    }

    if (fileNameAction == FNAME_RENAME && fileOpSourcePath.length()) {
      String parent = fmParentDir(fileOpSourcePath);
      String destination = fmJoinPath(parent, clean);
      if (destination == fileOpSourcePath) {
        currentScreen = SCREEN_FILES; drawFilesScreen(); return;
      }
      if (LittleFS.exists(destination)) {
        showMessage(T("ИМЯ УЖЕ ЗАНЯТО", "NAME EXISTS"), "", COLOR_RED, 650);
        currentScreen = SCREEN_FILE_NAME_INPUT; drawFileNameInput(); return;
      }
      bool ok = LittleFS.rename(fileOpSourcePath, destination);
      if (ok) fmUpdateWallpaperPath(fileOpSourcePath, destination);
      showMessage(ok ? T("ПЕРЕИМЕНОВАНО", "RENAMED") : T("ОШИБКА", "ERROR"), "", ok ? COLOR_GREEN : COLOR_RED, 450);
      refreshFileList(); refreshFmList();
      currentScreen = SCREEN_FILES; drawFilesScreen();
      return;
    }
  }

  fileNameMultiTap(key);
}

bool fmCopyRecursive(const String& source, const String& destination) {
  File src = LittleFS.open(source, "r");
  if (!src) return false;
  bool dir = src.isDirectory();

  if (!dir) {
    File dst = LittleFS.open(destination, "w");
    if (!dst) { src.close(); return false; }
    uint8_t buf[256];
    bool ok = true;
    while (src.available()) {
      size_t n = src.read(buf, sizeof(buf));
      if (!n) break;
      if (dst.write(buf, n) != n) { ok = false; break; }
    }
    src.close(); dst.close();
    if (!ok) LittleFS.remove(destination);
    return ok;
  }

  src.close();
  if (!LittleFS.exists(destination) && !LittleFS.mkdir(destination)) return false;
  File srcDir = LittleFS.open(source, "r");
  if (!srcDir) return false;
  File child = srcDir.openNextFile();
  while (child) {
    String childSource = String(child.name());
    if (!childSource.startsWith("/")) childSource = fmJoinPath(source, childSource);
    String childDest = fmJoinPath(destination, fmBaseName(childSource));
    child.close();
    if (!fmCopyRecursive(childSource, childDest)) {
      srcDir.close();
      fmDeleteRecursive(destination);
      return false;
    }
    child = srcDir.openNextFile();
  }
  srcDir.close();
  return true;
}

String fmUniqueDestination(const String& dir, const String& name, bool isDir) {
  String direct = fmJoinPath(dir, name);
  if (!LittleFS.exists(direct)) return direct;

  String stem = name, ext = "";
  if (!isDir) {
    int dot = name.lastIndexOf('.');
    if (dot > 0) { stem = name.substring(0, dot); ext = name.substring(dot); }
  }
  for (int i = 1; i <= 99; i++) {
    String candidate = fmJoinPath(dir, stem + "_" + String(i) + ext);
    if (!LittleFS.exists(candidate)) return candidate;
  }
  return "";
}

void refreshFolderPicker() {
  folderCount = 0;
  File dir = LittleFS.open(folderPickerDir, "r");
  if (!dir || !dir.isDirectory()) {
    if (dir) dir.close();
    folderPickerDir = "/";
    dir = LittleFS.open("/", "r");
  }
  if (!dir) return;

  File child = dir.openNextFile();
  while (child && folderCount < FM_MAX_ITEMS) {
    if (child.isDirectory()) {
      String p = String(child.name());
      if (!p.startsWith("/")) p = fmJoinPath(folderPickerDir, p);
      folderPaths[folderCount] = p;
      folderNames[folderCount] = fmBaseName(p);
      folderCount++;
    }
    child.close(); child = dir.openNextFile();
  }
  dir.close();
  if (folderSelected >= folderCount) folderSelected = folderCount - 1;
  if (folderSelected < 0) folderSelected = 0;
  if (folderTop > folderSelected) folderTop = folderSelected;
}

void drawFolderPicker() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, folderPickerMove ? T("ПЕРЕМЕСТИТЬ В", "MOVE TO") : T("КОПИРОВАТЬ В", "COPY TO"), COLOR_TEXT);
  drawUtf8(4, 27, utf8Truncate(folderPickerDir, 29), COLOR_GREEN, u8g2_font_5x7_t_cyrillic);

  tft.fillRect(0, 29, 160, 77, COLOR_BG);
  if (folderCount == 0) {
    uiText(31, 63, T("НЕТ ВЛОЖЕННЫХ ПАПОК", "NO SUBFOLDERS"), COLOR_GRAY);
  } else {
    const int visible = 4;
    if (folderSelected < folderTop) folderTop = folderSelected;
    if (folderSelected >= folderTop + visible) folderTop = folderSelected - visible + 1;
    for (int row = 0; row < visible; row++) {
      int i = folderTop + row; if (i >= folderCount) break;
      int y = 33 + row * 18;
      uint16_t c = COLOR_GRAY;
      if (i == folderSelected) { tft.fillRoundRect(2, y - 2, 156, 16, 3, COLOR_SELECT); c = COLOR_TEXT; }
      uiTextSmall(5, y + 8, "[D]", COLOR_YELLOW);
      drawUtf8(25, y + 8, utf8Truncate(folderNames[i], 22), c, u8g2_font_5x7_t_cyrillic);
    }
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(2, 116, T("# СЮДА A ВХОД B ВВЕРХ", "# HERE A OPEN B UP"), COLOR_GRAY);
  uiTextSmall(2, 126, T("2/8 ВЫБ D ОТМЕНА", "2/8 SEL D CANCEL"), COLOR_GRAY);
}

void executeCopyMoveTo(const String& destinationDir) {
  if (!fileOpSourcePath.length() || !LittleFS.exists(fileOpSourcePath)) {
    showMessage(T("ИСТОЧНИК НЕ НАЙДЕН", "SOURCE MISSING"), "", COLOR_RED, 650);
    currentScreen = SCREEN_FILES; refreshFmList(); drawFilesScreen(); return;
  }

  File sf = LittleFS.open(fileOpSourcePath, "r");
  bool sourceIsDir = sf && sf.isDirectory();
  if (sf) sf.close();

  String sourcePrefix = fileOpSourcePath;
  if (!sourcePrefix.endsWith("/")) sourcePrefix += "/";
  if (sourceIsDir && (destinationDir == fileOpSourcePath || destinationDir.startsWith(sourcePrefix))) {
    showMessage(T("НЕЛЬЗЯ ВНУТРЬ СЕБЯ", "CANNOT USE SUBFOLDER"), "", COLOR_RED, 850);
    currentScreen = SCREEN_FOLDER_PICKER; drawFolderPicker(); return;
  }

  String dest = fmUniqueDestination(destinationDir, fmBaseName(fileOpSourcePath), sourceIsDir);
  if (!dest.length()) {
    showMessage(T("НЕТ СВОБОДНОГО ИМЕНИ", "NO FREE NAME"), "", COLOR_RED, 650);
    currentScreen = SCREEN_FOLDER_PICKER; drawFolderPicker(); return;
  }

  if (folderPickerMove && fmParentDir(fileOpSourcePath) == destinationDir && dest != fileOpSourcePath) {


    showMessage(T("ФАЙЛ УЖЕ В ЭТОЙ ПАПКЕ", "ALREADY IN THIS FOLDER"), "", COLOR_YELLOW, 700);
    currentScreen = SCREEN_FILES; drawFilesScreen(); return;
  }

  bool ok = false;
  if (folderPickerMove) {
    ok = LittleFS.rename(fileOpSourcePath, dest);
    if (!ok) {

      ok = fmCopyRecursive(fileOpSourcePath, dest);
      if (ok) ok = fmDeleteRecursive(fileOpSourcePath);
    }
    if (ok) fmUpdateWallpaperPath(fileOpSourcePath, dest);
  } else {
    ok = fmCopyRecursive(fileOpSourcePath, dest);
  }

  refreshFileList(); refreshFmList();
  showMessage(ok ? (folderPickerMove ? T("ПЕРЕМЕЩЕНО", "MOVED") : T("СКОПИРОВАНО", "COPIED")) : T("ОШИБКА ОПЕРАЦИИ", "OPERATION ERROR"),
              ok ? fmBaseName(dest) : "", ok ? COLOR_GREEN : COLOR_RED, 650);
  currentScreen = SCREEN_FILES;
  drawFilesScreen();
}

void folderPickerKey(char key) {
  if (key == '2' && folderSelected > 0) { folderSelected--; if (folderSelected < folderTop) folderTop = folderSelected; drawFolderPicker(); return; }
  if (key == '8' && folderSelected < folderCount - 1) { folderSelected++; drawFolderPicker(); return; }
  if (key == 'A' && folderCount > 0) {
    folderPickerDir = folderPaths[folderSelected];
    folderSelected = 0; folderTop = 0;
    refreshFolderPicker(); drawFolderPicker(); return;
  }
  if (key == 'B') {
    if (folderPickerDir != "/") {
      folderPickerDir = fmParentDir(folderPickerDir);
      folderSelected = 0; folderTop = 0;
      refreshFolderPicker(); drawFolderPicker();
    }
    return;
  }
  if (key == '#') { executeCopyMoveTo(folderPickerDir); return; }
  if (key == 'D') { currentScreen = SCREEN_FILE_OPERATIONS; drawFileOperations(); }
}

void drawFileInfo() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ИНФОРМАЦИЯ", "INFORMATION"), COLOR_TEXT);

  if (!fileOpSourcePath.length() || !LittleFS.exists(fileOpSourcePath)) {
    uiText(25, 62, T("ФАЙЛ НЕ НАЙДЕН", "ITEM NOT FOUND"), COLOR_RED);
  } else {
    File f = LittleFS.open(fileOpSourcePath, "r");
    bool dir = f && f.isDirectory();
    size_t sz = (!dir && f) ? f.size() : 0;
    int children = 0;
    if (dir && f) {
      File c = f.openNextFile();
      while (c) { children++; c.close(); c = f.openNextFile(); }
    }
    if (f) f.close();

    uiTextSmall(5, 31, T("ИМЯ", "NAME"), COLOR_GRAY);
    drawUtf8(44, 31, utf8Truncate(fmBaseName(fileOpSourcePath), 22), COLOR_TEXT, u8g2_font_5x7_t_cyrillic);
    uiTextSmall(5, 44, T("ТИП", "TYPE"), COLOR_GRAY);
    uiTextSmall(44, 44, dir ? T("ПАПКА", "FOLDER") : T("ФАЙЛ", "FILE"), dir ? COLOR_YELLOW : COLOR_GREEN);
    if (dir) {
      uiTextSmall(5, 57, T("ОБЪЕКТОВ", "ITEMS"), COLOR_GRAY);
      tft.setTextSize(1); tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.setCursor(70, 50); tft.print(children);
    } else {
      uiTextSmall(5, 57, T("РАЗМЕР", "SIZE"), COLOR_GRAY);
      uiTextSmall(55, 57, formatBytes(sz), COLOR_TEXT);
      uiTextSmall(5, 70, T("РАСШИР.", "EXT"), COLOR_GRAY);
      uiTextSmall(55, 70, fmExtension(fileOpSourcePath), COLOR_TEXT);
    }
    uiTextSmall(5, 84, T("ПУТЬ", "PATH"), COLOR_GRAY);
    drawUtf8(5, 96, utf8Truncate(fileOpSourcePath, 31), COLOR_ACCENT, u8g2_font_5x7_t_cyrillic);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(5, 123, T("D/B - НАЗАД", "D/B - BACK"), COLOR_GRAY);
}

void fileInfoKey(char key) {
  if (key == 'D' || key == 'B' || key == 'A' || key == '#') {
    currentScreen = SCREEN_FILE_OPERATIONS;
    drawFileOperations();
  }
}




String uptimeString() {
  uint32_t total = millis() / 1000, h = total / 3600, m = (total % 3600) / 60, s = total % 60;
  char b[20]; snprintf(b, sizeof(b), "%02lu:%02lu:%02lu", (unsigned long)h, (unsigned long)m, (unsigned long)s); return String(b);
}

void drawSystemInfo() {
  tft.fillScreen(COLOR_BG); tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("О СИСТЕМЕ", "SYSTEM INFO"), COLOR_TEXT);
  tft.setTextSize(1); tft.setTextColor(COLOR_TEXT); tft.setCursor(137, 5); tft.print(systemPage + 1); tft.print("/2");

  int y = 30;
  auto row = [&](const String& k, const String& v) {

    uiText(4, y, utf8Truncate(k, 12), COLOR_GRAY);
    uiText(86, y, utf8Truncate(v, 12), COLOR_GREEN);
    y += 14;
  };

  if (systemPage == 0) {
    row(T("ЧИП", "CHIP"), String(ESP.getChipModel()));
    row(T("РЕВ/ЯДРА", "REV/CORES"), String(ESP.getChipRevision()) + "/" + String(ESP.getChipCores()));
    row(T("ЧАСТОТА CPU", "CPU FREQ"), String(ESP.getCpuFreqMHz()) + " MHz");
    row(T("СВОБ. HEAP", "FREE HEAP"), formatBytes(ESP.getFreeHeap()));
    row(T("МИН. HEAP", "MIN HEAP"), formatBytes(ESP.getMinFreeHeap()));
    row("PSRAM", ESP.getPsramSize() ? formatBytes(ESP.getPsramSize()) : T("НЕТ", "NONE"));
  } else {
    size_t total = LittleFS.totalBytes(), used = LittleFS.usedBytes();
    row("FLASH", formatBytes(ESP.getFlashChipSize()));
    row(T("ПРОШИВКА", "SKETCH"), formatBytes(ESP.getSketchSize()));
    row(T("СВОБ. ФС", "FS FREE"), formatBytes(total >= used ? total - used : 0));
    row("WI-FI", WiFi.status() == WL_CONNECTED ? WiFi.SSID() : T("НЕТ", "NONE"));
    row("RSSI", WiFi.status() == WL_CONNECTED ? String(WiFi.RSSI()) + " dBm" : "-");
    row(T("РАБОТА", "UPTIME"), uptimeString());
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(8, 123, T("2/8 СТРАНИЦА  D НАЗАД", "2/8 PAGE      D BACK"), COLOR_GRAY);
}

void systemKey(char key) {
  if (key == '2' || key == '8') {
    systemPage = 1 - systemPage; drawSystemInfo(); return;
  }
  if (key == 'D' || key == 'B') { currentScreen = SCREEN_SETTINGS; drawSettings(); }
}













String httpHtmlEscape(const String& text) {
  String out;
  out.reserve(text.length() + 12);
  for (size_t i = 0; i < text.length(); i++) {
    char c = text[i];
    if (c == '&') out += F("&amp;");
    else if (c == '<') out += F("&lt;");
    else if (c == '>') out += F("&gt;");
    else if (c == '"') out += F("&quot;");
    else if (c == '\'') out += F("&#39;");
    else out += c;
  }
  return out;
}

String httpUrlEncode(const String& text) {
  const char* hex = "0123456789ABCDEF";
  String out;
  out.reserve(text.length() * 2);
  for (size_t i = 0; i < text.length(); i++) {
    uint8_t c = (uint8_t)text[i];
    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        c == '-' || c == '_' || c == '.' || c == '~') {
      out += (char)c;
    } else {
      out += '%';
      out += hex[(c >> 4) & 0x0F];
      out += hex[c & 0x0F];
    }
  }
  return out;
}

String httpSafeBaseName(String name) {
  name.trim();
  name.replace("\\", "/");
  int slash = name.lastIndexOf('/');
  if (slash >= 0) name = name.substring(slash + 1);
  name.replace("..", "_");
  name.replace("/", "_");
  if (!name.length()) name = "UPLOAD.BIN";
  return name;
}

String httpUniqueUploadPath(const String& baseName) {
  String base = httpSafeBaseName(baseName);
  String path = "/" + base;
  if (!LittleFS.exists(path)) return path;

  int dot = base.lastIndexOf('.');
  String stem = (dot > 0) ? base.substring(0, dot) : base;
  String ext = (dot > 0) ? base.substring(dot) : "";

  for (int i = 1; i <= 99; i++) {
    path = "/" + stem + "_" + String(i) + ext;
    if (!LittleFS.exists(path)) return path;
  }

  return "/UPLOAD_" + String((unsigned long)millis()) + ext;
}

String httpContentType(const String& path) {
  String p = path;
  p.toLowerCase();
  if (p.endsWith(".txt") || p.endsWith(".log") || p.endsWith(".csv")) return "text/plain; charset=utf-8";
  if (p.endsWith(".html") || p.endsWith(".htm")) return "text/html; charset=utf-8";
  if (p.endsWith(".json")) return "application/json";
  if (p.endsWith(".png")) return "image/png";
  if (p.endsWith(".jpg") || p.endsWith(".jpeg")) return "image/jpeg";
  if (p.endsWith(".pdf")) return "application/pdf";
  return "application/octet-stream";
}

bool httpIsAuthorized() {
  return httpServer.hasArg("pin") && httpServer.arg("pin") == httpPin;
}

void httpSendLoginPage(bool wrongPin) {
  String page; page.reserve(2100);
  page += F("<!doctype html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>");
  page += F("<style>body{font-family:system-ui,sans-serif;max-width:420px;margin:40px auto;padding:0 16px;background:#111;color:#eee}.box{border:1px solid #444;border-radius:12px;padding:18px}input,button{font-size:18px;padding:10px;margin-top:10px;width:100%;box-sizing:border-box}.bad{color:#ff6b6b}</style></head><body><div class='box'>");
  if (systemLanguage == LANG_RU) {
    page += F("<title>PPC OS — вход</title><h2>PPC OS — обмен файлами</h2>");
    if (wrongPin) page += F("<p class='bad'>Неверный PIN-код</p>");
    page += F("<form method='GET' action='/'><label>PIN-код</label><input name='pin' inputmode='numeric' pattern='[0-9]*' type='password' required autofocus><button type='submit'>Открыть</button></form>");
  } else {
    page += F("<title>PPC OS — login</title><h2>PPC OS — file share</h2>");
    if (wrongPin) page += F("<p class='bad'>Wrong PIN</p>");
    page += F("<form method='GET' action='/'><label>PIN</label><input name='pin' inputmode='numeric' pattern='[0-9]*' type='password' required autofocus><button type='submit'>Open</button></form>");
  }
  page += F("</div></body></html>");
  httpServer.send(200, "text/html; charset=utf-8", page);
}

void httpHandleIndex() {
  httpRequestCount++;
  if (!httpIsAuthorized()) { httpSendLoginPage(httpServer.hasArg("pin")); return; }
  refreshFileList();
  size_t total = LittleFS.totalBytes(), used = LittleFS.usedBytes(), freeSpace = total >= used ? total - used : 0;
  String page; page.reserve(5400 + fileCount * 200);
  page += F("<!doctype html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><style>");
  page += F("body{font-family:system-ui,sans-serif;max-width:760px;margin:24px auto;padding:0 14px;background:#111;color:#eee}a{color:#67d8ff}table{width:100%;border-collapse:collapse}td,th{padding:8px;border-bottom:1px solid #333;text-align:left}.box{border:1px solid #444;border-radius:10px;padding:14px;margin:14px 0}.muted{color:#aaa}button{padding:8px 14px}</style></head><body>");
  if (systemLanguage == LANG_RU) {
    page += F("<title>PPC OS — файлы</title><h2>PPC OS — обмен файлами</h2><div class='box'><b>Устройство:</b> PPC OS 2.8.5<br><b>IP:</b> "); page += WiFi.localIP().toString();
    page += F("<br><b>Хранилище:</b> "); page += formatBytes(used); page += F(" занято / "); page += formatBytes(total); page += F(" всего, "); page += formatBytes(freeSpace); page += F(" свободно</div>");
    page += F("<div class='box'><h3>Загрузить файл в PPC</h3><form method='POST' action='/upload?pin="); page += httpUrlEncode(httpPin); page += F("' enctype='multipart/form-data'><input type='file' name='file' required> <button type='submit'>Загрузить</button></form><p class='muted'>Если имя уже занято, PPC OS создаст копию с номером.</p></div>");
    page += F("<div class='box'><h3>Файлы</h3><table><tr><th>Имя</th><th>Размер</th><th></th></tr>");
  } else {
    page += F("<title>PPC OS — files</title><h2>PPC OS — file share</h2><div class='box'><b>Device:</b> PPC OS 2.8.5<br><b>IP:</b> "); page += WiFi.localIP().toString();
    page += F("<br><b>Storage:</b> "); page += formatBytes(used); page += F(" used / "); page += formatBytes(total); page += F(" total, "); page += formatBytes(freeSpace); page += F(" free</div>");
    page += F("<div class='box'><h3>Upload file to PPC</h3><form method='POST' action='/upload?pin="); page += httpUrlEncode(httpPin); page += F("' enctype='multipart/form-data'><input type='file' name='file' required> <button type='submit'>Upload</button></form><p class='muted'>If the name exists, PPC OS creates a numbered copy.</p></div>");
    page += F("<div class='box'><h3>Files</h3><table><tr><th>Name</th><th>Size</th><th></th></tr>");
  }
  for (int i = 0; i < fileCount; i++) {
    String path = fileNames[i], name = path; if (name.startsWith("/")) name = name.substring(1);
    page += F("<tr><td>"); page += httpHtmlEscape(name); page += F("</td><td>"); page += formatBytes(fileSizes[i]);
    page += F("</td><td><a href='/download?name="); page += httpUrlEncode(path); page += F("&pin="); page += httpUrlEncode(httpPin); page += F("'>");
    page += systemLanguage == LANG_RU ? "Скачать" : "Download";
    page += F("</a></td></tr>");
  }
  if (!fileCount) page += systemLanguage == LANG_RU ? F("<tr><td colspan='3' class='muted'>Файлов нет</td></tr>") : F("<tr><td colspan='3' class='muted'>No files</td></tr>");
  page += F("</table></div><p class='muted'>");
  page += systemLanguage == LANG_RU ? "Телефон/ПК и PPC должны быть подключены к одной сети Wi-Fi." : "Phone/PC and PPC must be connected to the same Wi-Fi network.";
  page += F("</p></body></html>");
  httpServer.send(200, "text/html; charset=utf-8", page);
}

void httpHandleDownload() {
  httpRequestCount++;
  if (!httpIsAuthorized()) {
    httpServer.send(403, "text/plain", T("Неверный PIN", "Wrong PIN"));
    return;
  }
  if (!httpServer.hasArg("name")) {
    httpServer.send(400, "text/plain", T("Не указано имя файла", "File name missing"));
    return;
  }

  String requested = httpServer.arg("name");
  String base = httpSafeBaseName(requested);
  String path = "/" + base;

  if (!LittleFS.exists(path)) {
    httpServer.send(404, "text/plain", T("Файл не найден", "File not found"));
    return;
  }

  File file = LittleFS.open(path, "r");
  if (!file) {
    httpServer.send(500, "text/plain", T("Ошибка открытия", "Open error"));
    return;
  }

  String safeHeaderName = base;
  safeHeaderName.replace("\"", "_");
  httpServer.sendHeader("Content-Disposition", "attachment; filename=\"" + safeHeaderName + "\"");
  httpServer.sendHeader("Cache-Control", "no-store");
  httpServer.streamFile(file, httpContentType(path));
  file.close();
}

void httpHandleUploadData() {
  HTTPUpload& upload = httpServer.upload();

  if (upload.status == UPLOAD_FILE_START) {
    httpUploadFailed = !httpIsAuthorized();
    httpUploadBytes = 0;
    httpUploadPath = "";

    if (httpUploadFile) httpUploadFile.close();
    if (!httpUploadFailed) {
      httpUploadPath = httpUniqueUploadPath(upload.filename);
      httpUploadFile = LittleFS.open(httpUploadPath, "w");
      if (!httpUploadFile) httpUploadFailed = true;
    }
  }

  else if (upload.status == UPLOAD_FILE_WRITE) {
    if (!httpUploadFailed && httpUploadFile) {
      size_t written = httpUploadFile.write(upload.buf, upload.currentSize);
      httpUploadBytes += written;
      if (written != upload.currentSize) httpUploadFailed = true;
    }
  }

  else if (upload.status == UPLOAD_FILE_END) {
    if (httpUploadFile) httpUploadFile.close();
    if (httpUploadFailed) {
      if (httpUploadPath.length()) LittleFS.remove(httpUploadPath);
    }
    refreshFileList();
  }

  else if (upload.status == UPLOAD_FILE_ABORTED) {
    if (httpUploadFile) httpUploadFile.close();
    if (httpUploadPath.length()) LittleFS.remove(httpUploadPath);
    httpUploadFailed = true;
  }
}

void httpHandleUploadDone() {
  httpRequestCount++;
  if (!httpIsAuthorized()) {
    httpServer.send(403, "text/plain", T("Неверный PIN", "Wrong PIN"));
  } else if (httpUploadFailed) {
    httpServer.send(507, "text/plain", T("Ошибка загрузки или нет места", "Upload failed or no space"));
  } else {
    httpServer.sendHeader("Location", "/?pin=" + httpUrlEncode(httpPin));
    httpServer.send(303, "text/plain", "");
  }
}

void httpHandleNotFound() {
  httpRequestCount++;
  httpServer.send(404, "text/plain", T("PPC OS: не найдено", "PPC OS: not found"));
}

void configureHttpRoutes() {
  if (httpRoutesConfigured) return;

  httpServer.on("/", HTTP_GET, httpHandleIndex);
  httpServer.on("/download", HTTP_GET, httpHandleDownload);
  httpServer.on("/upload", HTTP_POST, httpHandleUploadDone, httpHandleUploadData);
  httpServer.onNotFound(httpHandleNotFound);
  httpRoutesConfigured = true;
}

void startHttpServer() {
  if (httpServerRunning) return;
  if (WiFi.status() != WL_CONNECTED) return;

  configureHttpRoutes();
  httpServer.begin();
  httpServerRunning = true;
}

void stopHttpServer() {
  if (!httpServerRunning) return;
  httpServer.stop();
  httpServerRunning = false;
  if (httpUploadFile) httpUploadFile.close();
}

void drawHttpShare() {
  tft.fillScreen(COLOR_BG); tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("WI-FI / HTTP ОБМЕН", "WI-FI / HTTP SHARE"), COLOR_TEXT);
  int y = 27;
  if (WiFi.status() != WL_CONNECTED) {
    uiText(5, y + 7, T("НЕТ WI-FI", "NO WI-FI"), COLOR_RED); uiTextSmall(5, y + 23, T("СНАЧАЛА ПОДКЛЮЧИТЕСЬ", "CONNECT FIRST"), COLOR_GRAY); uiTextSmall(5, y + 36, T("В РАЗДЕЛЕ СЕТЕЙ WI-FI", "IN WI-FI NETWORKS"), COLOR_GRAY);
  } else {
    uiText(5, y + 7, httpServerRunning ? T("СЕРВЕР: ВКЛ", "SERVER: ON") : T("СЕРВЕР: ВЫКЛ", "SERVER: OFF"), httpServerRunning ? COLOR_GREEN : COLOR_YELLOW);
    tft.setTextSize(1); tft.setTextColor(COLOR_GRAY); tft.setCursor(5, y + 14); tft.print("http://"); tft.setTextColor(COLOR_ACCENT); tft.print(WiFi.localIP());
    uiTextSmall(5, y + 36, "PIN:", COLOR_GRAY); tft.setTextColor(COLOR_YELLOW); tft.setCursor(31, y + 29); tft.print(httpPin);
    size_t total = LittleFS.totalBytes(), used = LittleFS.usedBytes(), freeSpace = total >= used ? total - used : 0;
    uiTextSmall(5, y + 51, T("СВОБ:", "FREE:"), COLOR_GRAY); tft.setTextColor(COLOR_GREEN); tft.setCursor(40, y + 44); tft.print(formatBytes(freeSpace));
    uiTextSmall(88, y + 51, T("ЗАПР:", "REQ:"), COLOR_GRAY); tft.setTextColor(COLOR_GRAY); tft.setCursor(121, y + 44); tft.print((unsigned long)httpRequestCount);
    uiTextSmall(5, y + 66, T("ОДНА СЕТЬ WI-FI", "SAME WI-FI NETWORK"), COLOR_GRAY);
  }
  tft.drawFastHLine(0, 108, 160, COLOR_GRAY); uiTextSmall(2, 123, T("A ВКЛ B PIN C ОБН D НАЗ", "A ON B PIN C REF D BACK"), COLOR_GRAY);
}

void drawHttpPinScreen() {
  tft.fillScreen(COLOR_BG); tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 12, T("PIN ДЛЯ HTTP", "HTTP PIN"), COLOR_TEXT); uiText(35, 41, T("Введите 4-6 цифр", "Enter 4-6 digits"), COLOR_GRAY);
  tft.drawRoundRect(25, 51, 110, 28, 4, COLOR_ACCENT); tft.setTextSize(2); tft.setTextColor(COLOR_YELLOW, COLOR_BG);
  String shown = httpPinEdit.length() ? httpPinEdit : "----"; int x = 80 - (shown.length() * 12) / 2; tft.setCursor(x, 58); tft.print(shown);
  uiTextSmall(12, 97, T("B УДАЛ   C СОХР   D ОТМЕНА", "B DEL C SAVE D CANCEL"), COLOR_GRAY);
}

void httpPinKey(char key) {
  if (key >= '0' && key <= '9') {
    if (httpPinEdit.length() < 6) httpPinEdit += key;
    drawHttpPinScreen(); return;
  }
  if (key == 'B') {
    if (httpPinEdit.length()) httpPinEdit.remove(httpPinEdit.length() - 1);
    drawHttpPinScreen(); return;
  }
  if (key == 'C' || key == '#') {
    if (httpPinEdit.length() < 4) {
      showMessage(T("PIN СЛИШКОМ КОРОТКИЙ", "PIN TOO SHORT"), T("НУЖНО 4-6 ЦИФР", "USE 4-6 DIGITS"), COLOR_RED, 800);
      drawHttpPinScreen(); return;
    }
    httpPin = httpPinEdit;
    preferences.putString("httpPin", httpPin);
    currentScreen = SCREEN_HTTP_SHARE;
    drawHttpShare(); return;
  }
  if (key == 'D') {
    currentScreen = SCREEN_HTTP_SHARE;
    drawHttpShare();
  }
}

void httpShareKey(char key) {
  if (key == 'A' || key == '#') {
    if (WiFi.status() != WL_CONNECTED) {
      showMessage(T("НЕТ WI-FI", "NO WI-FI"), T("СНАЧАЛА ПОДКЛЮЧИТЕСЬ", "CONNECT FIRST"), COLOR_RED, 900);
      drawHttpShare();
      return;
    }
    if (httpServerRunning) stopHttpServer();
    else startHttpServer();
    drawHttpShare();
    return;
  }

  if (key == 'B') {
    httpPinEdit = httpPin;
    currentScreen = SCREEN_HTTP_PIN;
    drawHttpPinScreen();
    return;
  }

  if (key == 'C') {
    drawHttpShare();
    return;
  }

  if (key == 'D') {

    currentScreen = SCREEN_WIFI_MENU;
    drawWiFiMenu();
  }
}




String gamesMenuItemText(int i) {
  if (i < 0 || i >= GAMES_MENU_ITEMS) return "";
  return String(systemLanguage == LANG_RU ? gamesMenuItemsRu[i] : gamesMenuItemsEn[i]);
}

void drawGamesMenu() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("ИГРЫ", "GAMES"), COLOR_TEXT);

  for (int i = 0; i < GAMES_MENU_ITEMS; i++) {
    int y = 35 + i * 31;
    uint16_t c = COLOR_GRAY;
    if (i == gamesMenuSelected) {
      tft.fillRoundRect(5, y - 7, 150, 24, 3, COLOR_SELECT);
      c = COLOR_TEXT;
    }
    tft.setTextSize(1); tft.setTextColor(c);
    tft.setCursor(10, y); tft.print(i + 1); tft.print(".");
    uiText(28, y + 8, gamesMenuItemText(i), c);
  }

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  uiTextSmall(4, 123, T("2/8 ВЫБОР A ОТКР D НАЗ", "2/8 MOVE A OPEN D BACK"), COLOR_GRAY);
}

void gamesMenuKey(char key) {
  if (key == '2' || key == '8') {
    gamesMenuSelected = 1 - gamesMenuSelected;
    drawGamesMenu();
    return;
  }
  if (key == 'A' || key == '#') {
    if (gamesMenuSelected == 0) {
      currentScreen = SCREEN_PONG;
      resetPongGame();
      drawPong();
    } else {
      currentScreen = SCREEN_SUDOKU;
      sudokuNewGame();
      drawSudoku();
    }
    return;
  }
  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_MENU;
    drawMenu();
  }
}




void sudokuLoadPuzzle(int index) {
  static const uint8_t solutions[3][16] = {
    {1,2,3,4, 3,4,1,2, 2,1,4,3, 4,3,2,1},
    {4,1,2,3, 2,3,4,1, 1,4,3,2, 3,2,1,4},
    {2,3,1,4, 1,4,2,3, 3,2,4,1, 4,1,3,2}
  };
  static const uint8_t puzzles[3][16] = {
    {1,0,3,0, 0,4,0,2, 2,0,4,0, 0,3,0,1},
    {4,0,0,3, 0,3,4,0, 1,0,3,0, 0,2,0,4},
    {0,3,0,4, 1,0,2,0, 0,2,0,1, 4,0,3,0}
  };

  if (index < 0) index = 0;
  index %= 3;
  sudokuPuzzleIndex = index;
  for (int i = 0; i < 16; i++) {
    sudokuSolution[i] = solutions[index][i];
    sudokuBoard[i] = puzzles[index][i];
    sudokuFixed[i] = puzzles[index][i] != 0;
  }
  sudokuCursor = 0;
  while (sudokuCursor < 15 && sudokuFixed[sudokuCursor]) sudokuCursor++;
  sudokuSolved = false;
}

void sudokuNewGame() {
  sudokuLoadPuzzle(random(0, 3));
}

bool sudokuCellConflicts(int cell) {
  uint8_t v = sudokuBoard[cell];
  if (v == 0) return false;
  int row = cell / 4;
  int col = cell % 4;

  for (int c = 0; c < 4; c++) {
    int i = row * 4 + c;
    if (i != cell && sudokuBoard[i] == v) return true;
  }
  for (int r = 0; r < 4; r++) {
    int i = r * 4 + col;
    if (i != cell && sudokuBoard[i] == v) return true;
  }
  int br = (row / 2) * 2;
  int bc = (col / 2) * 2;
  for (int r = br; r < br + 2; r++) {
    for (int c = bc; c < bc + 2; c++) {
      int i = r * 4 + c;
      if (i != cell && sudokuBoard[i] == v) return true;
    }
  }
  return false;
}

bool sudokuIsComplete() {
  for (int i = 0; i < 16; i++) if (sudokuBoard[i] == 0) return false;
  return true;
}

bool sudokuMatchesSolution() {
  for (int i = 0; i < 16; i++) if (sudokuBoard[i] != sudokuSolution[i]) return false;
  return true;
}

void drawSudoku() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  uiText(5, 13, T("СУДОКУ 4x4", "SUDOKU 4x4"), COLOR_TEXT);

  const int gx = 4;
  const int gy = 20;
  const int cell = 21;
  const int size = cell * 4;


  tft.fillRect(gx, gy, size, size, COLOR_BG);
  for (int i = 0; i <= 4; i++) {
    uint16_t c = (i == 0 || i == 4 || i == 2) ? COLOR_TEXT : COLOR_GRAY;
    int thick = (i == 0 || i == 4 || i == 2) ? 2 : 1;
    for (int k = 0; k < thick; k++) {
      tft.drawFastVLine(gx + i * cell + (i == 4 ? -k : k), gy, size, c);
      tft.drawFastHLine(gx, gy + i * cell + (i == 4 ? -k : k), size, c);
    }
  }


  for (int i = 0; i < 16; i++) {
    int r = i / 4;
    int c = i % 4;
    int x = gx + c * cell;
    int y = gy + r * cell;

    if (i == sudokuCursor && !sudokuSolved) {
      tft.drawRect(x + 2, y + 2, cell - 4, cell - 4, COLOR_ACCENT);
      tft.drawRect(x + 3, y + 3, cell - 6, cell - 6, COLOR_ACCENT);
    }

    if (sudokuBoard[i] != 0) {
      uint16_t color = sudokuFixed[i] ? COLOR_YELLOW : (sudokuCellConflicts(i) ? COLOR_RED : COLOR_GREEN);
      tft.setTextSize(2);
      tft.setTextColor(color, COLOR_BG);
      tft.setCursor(x + 7, y + 3);
      tft.print((int)sudokuBoard[i]);
    }
  }


  uiTextSmall(94, 30, T("1-4 ЦИФРА", "1-4 DIGIT"), COLOR_TEXT);
  uiTextSmall(94, 43, T("A/B ВЕРТ", "A/B VERT"), COLOR_GRAY);
  uiTextSmall(94, 56, T("C/D ГОР", "C/D HOR"), COLOR_GRAY);
  uiTextSmall(94, 69, T("* СТЕРЕТЬ", "* CLEAR"), COLOR_GRAY);
  uiTextSmall(94, 82, T("0 НОВАЯ", "0 NEW"), COLOR_GRAY);
  uiTextSmall(94, 95, T("# НАЗАД", "# BACK"), COLOR_GRAY);

  if (sudokuSolved) {
    tft.fillRect(3, 108, 154, 18, COLOR_BG);
    drawUiCentered(T("РЕШЕНО! A - НОВАЯ", "SOLVED! A - NEW"), 123, COLOR_GREEN, u8g2_font_5x7_t_cyrillic, 5);
  }
}

void sudokuKey(char key) {

  if (key == '#') {
    currentScreen = SCREEN_GAMES_MENU;
    drawGamesMenu();
    return;
  }

  if (key == '0') {
    sudokuPuzzleIndex = (sudokuPuzzleIndex + 1) % 3;
    sudokuLoadPuzzle(sudokuPuzzleIndex);
    drawSudoku();
    return;
  }

  if (sudokuSolved) return;

  int row = sudokuCursor / 4;
  int col = sudokuCursor % 4;
  int next = sudokuCursor;


  if (key == 'A') { row = (row + 3) % 4; next = row * 4 + col; }
  else if (key == 'B') { row = (row + 1) % 4; next = row * 4 + col; }
  else if (key == 'C') { col = (col + 3) % 4; next = row * 4 + col; }
  else if (key == 'D') { col = (col + 1) % 4; next = row * 4 + col; }

  if (next != sudokuCursor) {
    sudokuCursor = next;
    drawSudoku();
    return;
  }

  if (key >= '1' && key <= '4') {
    if (!sudokuFixed[sudokuCursor]) {
      sudokuBoard[sudokuCursor] = key - '0';
      if (sudokuIsComplete() && sudokuMatchesSolution()) sudokuSolved = true;
      drawSudoku();
    }
    return;
  }

  if (key == '*') {
    if (!sudokuFixed[sudokuCursor]) {
      sudokuBoard[sudokuCursor] = 0;
      drawSudoku();
    }
  }
}




void drawPongScore() {
  tft.fillRect(104, 0, 56, 18, COLOR_SELECT);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT, COLOR_SELECT);
  tft.setCursor(116, 5);
  tft.print(pongPlayerScore);
  tft.print(":");
  tft.print(pongAiScore);
}

void drawPongStatus(const String& text, uint16_t color) {
  tft.fillRect(24, 52, 112, 18, COLOR_BG); tft.drawRect(24, 52, 112, 18, color);
  int x = 80 - (utf8Count(text) * 5) / 2; if (x < 27) x = 27; uiText(x, 65, text, color);
}

void resetPongBall(int direction) {
  pongBallX = 78;
  pongBallY = 60;
  pongBallVX = direction >= 0 ? 2 : -2;
  pongBallVY = random(0, 2) ? 1 : -1;
}

void resetPongGame() {
  pongPlayerY = 52;
  pongAiY = 52;
  pongPlayerScore = 0;
  pongAiScore = 0;
  pongPaused = false;
  pongGameOver = false;
  pongLastFrame = millis();
  resetPongBall(random(0, 2) ? 1 : -1);
}

void drawPong() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, 160, 18, COLOR_SELECT);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT, COLOR_SELECT);
  tft.setCursor(5, 5);
  uiText(5, 12, T("ПИНГ-ПОНГ", "PING PONG"), COLOR_TEXT);
  drawPongScore();

  tft.drawRect(0, PONG_FIELD_TOP, 160, PONG_FIELD_BOTTOM - PONG_FIELD_TOP + 1, COLOR_GRAY);
  tft.fillRect(PONG_PLAYER_X, pongPlayerY, PONG_PADDLE_W, PONG_PADDLE_H, COLOR_ACCENT);
  tft.fillRect(PONG_AI_X, pongAiY, PONG_PADDLE_W, PONG_PADDLE_H, COLOR_YELLOW);
  tft.fillRect(pongBallX, pongBallY, PONG_BALL_SIZE, PONG_BALL_SIZE, COLOR_TEXT);

  tft.drawFastHLine(0, 108, 160, COLOR_GRAY);
  tft.setTextColor(COLOR_GRAY, COLOR_BG);
  tft.setCursor(3, 116);
  uiTextSmall(2, 123, T("2/8 ДВИЖ A ПАУЗА D НАЗАД", "2/8 MOVE A PAUSE D BACK"), COLOR_GRAY);
}

void pongKey(char key) {
  if (key == '2') {
    tft.fillRect(PONG_PLAYER_X, pongPlayerY, PONG_PADDLE_W, PONG_PADDLE_H, COLOR_BG);
    pongPlayerY -= 9;
    if (pongPlayerY < PONG_FIELD_TOP + 2) pongPlayerY = PONG_FIELD_TOP + 2;
    tft.fillRect(PONG_PLAYER_X, pongPlayerY, PONG_PADDLE_W, PONG_PADDLE_H, COLOR_ACCENT);
    return;
  }

  if (key == '8') {
    tft.fillRect(PONG_PLAYER_X, pongPlayerY, PONG_PADDLE_W, PONG_PADDLE_H, COLOR_BG);
    pongPlayerY += 9;
    int maxY = PONG_FIELD_BOTTOM - PONG_PADDLE_H - 1;
    if (pongPlayerY > maxY) pongPlayerY = maxY;
    tft.fillRect(PONG_PLAYER_X, pongPlayerY, PONG_PADDLE_W, PONG_PADDLE_H, COLOR_ACCENT);
    return;
  }

  if (key == 'A' || key == '#') {
    if (pongGameOver) {
      resetPongGame();
      drawPong();
    } else {
      pongPaused = !pongPaused;
      if (pongPaused) drawPongStatus(T("ПАУЗА", "PAUSED"), COLOR_YELLOW);
      else {
        drawPong();
        pongLastFrame = millis();
      }
    }
    return;
  }

  if (key == 'D' || key == 'B') {
    currentScreen = SCREEN_GAMES_MENU;
    drawGamesMenu();
  }
}

void updatePong() {
  if (currentScreen != SCREEN_PONG || pongPaused || pongGameOver) return;
  unsigned long now = millis();
  if (now - pongLastFrame < PONG_FRAME_MS) return;
  pongLastFrame = now;

  int oldBallX = pongBallX;
  int oldBallY = pongBallY;
  int oldAiY = pongAiY;


  int aiCenter = pongAiY + PONG_PADDLE_H / 2;
  int ballCenter = pongBallY + PONG_BALL_SIZE / 2;
  if (ballCenter < aiCenter - 2) pongAiY--;
  else if (ballCenter > aiCenter + 2) pongAiY++;

  if (pongAiY < PONG_FIELD_TOP + 2) pongAiY = PONG_FIELD_TOP + 2;
  int maxAiY = PONG_FIELD_BOTTOM - PONG_PADDLE_H - 1;
  if (pongAiY > maxAiY) pongAiY = maxAiY;

  pongBallX += pongBallVX;
  pongBallY += pongBallVY;


  if (pongBallY <= PONG_FIELD_TOP + 2) {
    pongBallY = PONG_FIELD_TOP + 2;
    pongBallVY = abs(pongBallVY);
  }
  if (pongBallY + PONG_BALL_SIZE >= PONG_FIELD_BOTTOM - 1) {
    pongBallY = PONG_FIELD_BOTTOM - PONG_BALL_SIZE - 1;
    pongBallVY = -abs(pongBallVY);
  }


  if (pongBallVX < 0 &&
      pongBallX <= PONG_PLAYER_X + PONG_PADDLE_W &&
      pongBallX + PONG_BALL_SIZE >= PONG_PLAYER_X &&
      pongBallY + PONG_BALL_SIZE >= pongPlayerY &&
      pongBallY <= pongPlayerY + PONG_PADDLE_H) {
    pongBallX = PONG_PLAYER_X + PONG_PADDLE_W + 1;
    pongBallVX = abs(pongBallVX);
    int hit = ballCenter - pongPlayerY;
    if (hit < PONG_PADDLE_H / 3) pongBallVY = -2;
    else if (hit > (PONG_PADDLE_H * 2) / 3) pongBallVY = 2;
  }


  if (pongBallVX > 0 &&
      pongBallX + PONG_BALL_SIZE >= PONG_AI_X &&
      pongBallX <= PONG_AI_X + PONG_PADDLE_W &&
      pongBallY + PONG_BALL_SIZE >= pongAiY &&
      pongBallY <= pongAiY + PONG_PADDLE_H) {
    pongBallX = PONG_AI_X - PONG_BALL_SIZE - 1;
    pongBallVX = -abs(pongBallVX);
    int hit = ballCenter - pongAiY;
    if (hit < PONG_PADDLE_H / 3) pongBallVY = -2;
    else if (hit > (PONG_PADDLE_H * 2) / 3) pongBallVY = 2;
  }


  bool scored = false;
  if (pongBallX < 0) {
    pongAiScore++;
    scored = true;
    resetPongBall(1);
  } else if (pongBallX > 160) {
    pongPlayerScore++;
    scored = true;
    resetPongBall(-1);
  }


  tft.fillRect(oldBallX, oldBallY, PONG_BALL_SIZE, PONG_BALL_SIZE, COLOR_BG);
  if (oldAiY != pongAiY) {
    tft.fillRect(PONG_AI_X, oldAiY, PONG_PADDLE_W, PONG_PADDLE_H, COLOR_BG);
  }


  tft.drawRect(0, PONG_FIELD_TOP, 160, PONG_FIELD_BOTTOM - PONG_FIELD_TOP + 1, COLOR_GRAY);
  tft.fillRect(PONG_PLAYER_X, pongPlayerY, PONG_PADDLE_W, PONG_PADDLE_H, COLOR_ACCENT);
  tft.fillRect(PONG_AI_X, pongAiY, PONG_PADDLE_W, PONG_PADDLE_H, COLOR_YELLOW);
  tft.fillRect(pongBallX, pongBallY, PONG_BALL_SIZE, PONG_BALL_SIZE, COLOR_TEXT);

  if (scored) {
    drawPongScore();
    if (pongPlayerScore >= PONG_WIN_SCORE || pongAiScore >= PONG_WIN_SCORE) {
      pongGameOver = true;
      pongPaused = true;
      drawPongStatus(pongPlayerScore > pongAiScore ? T("ВЫ ПОБЕДИЛИ - A", "YOU WIN - A") : T("ПОБЕДИЛ ПК - A", "CPU WINS - A"), COLOR_YELLOW);
    }
  }
}




void handlePressedKey(char key) {
  if (currentScreen == SCREEN_DESKTOP) { desktopKey(key); return; }
  if (currentScreen == SCREEN_MENU) { menuKey(key); return; }
  if (currentScreen == SCREEN_CALCULATIONS_MENU) { calculationsMenuKey(key); return; }
  if (currentScreen == SCREEN_CALCULATOR) { calculatorKey(key); return; }
  if (currentScreen == SCREEN_PROGRAMMER_CALC) { programmerCalculatorKey(key); return; }
  if (currentScreen == SCREEN_PROGRAMMER_OPS) { programmerOperationsKey(key); return; }
  if (currentScreen == SCREEN_TRUTH_TABLE) { truthTableKey(key); return; }
  if (currentScreen == SCREEN_MATH_CALC) { mathCalculatorKey(key); return; }
  if (currentScreen == SCREEN_MATH_FUNCTIONS) { mathFunctionsKey(key); return; }
  if (currentScreen == SCREEN_FUNCTION_GRAPH) { functionGraphKey(key); return; }
  if (currentScreen == SCREEN_PASSWORD_GENERATOR) { passwordGeneratorKey(key); return; }
  if (currentScreen == SCREEN_HASH_CALC) { hashCalculatorKey(key); return; }
  if (currentScreen == SCREEN_PROGRAM_MENU) { programMenuKey(key); return; }
  if (currentScreen == SCREEN_PROGRAM_RUN) { programRunKey(key); return; }
  if (currentScreen == SCREEN_PROGRAM_HELP) { programHelpKey(key); return; }
  if (currentScreen == SCREEN_DATETIME_MENU) { dateTimeMenuKey(key); return; }
  if (currentScreen == SCREEN_CLOCK) {
    if (key == 'B' || key == 'D') { currentScreen = SCREEN_DATETIME_MENU; drawDateTimeMenu(); }
    return;
  }
  if (currentScreen == SCREEN_TIMER) { timerKey(key); return; }
  if (currentScreen == SCREEN_STOPWATCH) { stopwatchKey(key); return; }
  if (currentScreen == SCREEN_CALENDAR) { calendarKey(key); return; }
  if (currentScreen == SCREEN_CALENDAR_EVENT) { calendarEventKey(key); return; }
  if (currentScreen == SCREEN_WIFI_MENU) { wifiMenuKey(key); return; }
  if (currentScreen == SCREEN_WIFI) { wifiKey(key); return; }
  if (currentScreen == SCREEN_WIFI_PASSWORD) { passwordKey(key); return; }
  if (currentScreen == SCREEN_RECORDS_MENU) { recordsMenuKey(key); return; }
  if (currentScreen == SCREEN_NOTES) { notesKey(key); return; }
  if (currentScreen == SCREEN_SECURE_PIN) { securePinKey(key); return; }
  if (currentScreen == SCREEN_SECURE_NOTES) { secureNotesKey(key); return; }
  if (currentScreen == SCREEN_CONTACTS) { contactsKey(key); return; }
  if (currentScreen == SCREEN_CONTACT_VIEW) { contactViewKey(key); return; }
  if (currentScreen == SCREEN_CONTACT_EDIT) { contactEditorKey(key); return; }
  if (currentScreen == SCREEN_EDITOR) { editorKey(key); return; }
  if (currentScreen == SCREEN_FILES_MENU) { filesMenuKey(key); return; }
  if (currentScreen == SCREEN_FILES) { filesKey(key); return; }
  if (currentScreen == SCREEN_FILE_OPERATIONS) { fileOperationsKey(key); return; }
  if (currentScreen == SCREEN_FILE_NAME_INPUT) { fileNameInputKey(key); return; }
  if (currentScreen == SCREEN_FOLDER_PICKER) { folderPickerKey(key); return; }
  if (currentScreen == SCREEN_FILE_INFO) { fileInfoKey(key); return; }
  if (currentScreen == SCREEN_TEXT_PICKER) { filePickerKey(key, true); return; }
  if (currentScreen == SCREEN_TEXT_READER) { textReaderKey(key); return; }
  if (currentScreen == SCREEN_HEX_PICKER) { filePickerKey(key, false); return; }
  if (currentScreen == SCREEN_HEX_VIEWER) { hexViewerKey(key); return; }
  if (currentScreen == SCREEN_IMAGE_PICKER) { imagePickerKey(key); return; }
  if (currentScreen == SCREEN_IMAGE_VIEWER) { imageViewerKey(key); return; }
  if (currentScreen == SCREEN_SYSTEM) { systemKey(key); return; }
  if (currentScreen == SCREEN_HTTP_SHARE) { httpShareKey(key); return; }
  if (currentScreen == SCREEN_HTTP_PIN) { httpPinKey(key); return; }
  if (currentScreen == SCREEN_GAMES_MENU) { gamesMenuKey(key); return; }
  if (currentScreen == SCREEN_PONG) { pongKey(key); return; }
  if (currentScreen == SCREEN_SUDOKU) { sudokuKey(key); return; }
  if (currentScreen == SCREEN_SETTINGS) { settingsKey(key); return; }
  if (currentScreen == SCREEN_LANGUAGE) { languageKey(key); return; }
  if (currentScreen == SCREEN_THEME) { themeKey(key); return; }
  if (currentScreen == SCREEN_DESKTOP_SETTINGS) { desktopSettingsKey(key); return; }
  if (currentScreen == SCREEN_WALLPAPER_PICKER) { wallpaperPickerKey(key); return; }
  if (currentScreen == SCREEN_LOCK_SETTINGS) { lockSettingsKey(key); return; }
  if (currentScreen == SCREEN_LOCK_PIN_CHANGE) { lockPinChangeKey(key); return; }
  if (currentScreen == SCREEN_LOCK) { lockKey(key); return; }
}

void handleHeldKey(char key) {

  if (key == 'D') {
    if (currentScreen == SCREEN_PROGRAM_RUN) {
      programRunning = false;
      programPaused = false;
      programFinished = true;
      programStatus = T("ОСТАНОВЛЕНО", "STOPPED");
    }
    goHome();
    return;
  }

  if (currentScreen == SCREEN_CALCULATOR && key == '*') {
    currentScreen = SCREEN_CALCULATIONS_MENU; drawCalculationsMenu();
    return;
  }
}




void setup() {
  Serial.begin(115200);
  preferences.begin("ppcos", false);
  systemLanguage = preferences.getUChar("lang", 0) == 1 ? LANG_EN : LANG_RU;
  currentTheme = preferences.getUChar("theme", THEME_CLASSIC);
  if (currentTheme >= THEME_COUNT) currentTheme = THEME_CLASSIC;
  applyTheme(currentTheme);
  httpPin = preferences.getString("httpPin", "1234");
  if (httpPin.length() < 4 || httpPin.length() > 6) httpPin = "1234";
  wallpaperPath = preferences.getString("wallpaper", "");
  desktopShowClock = preferences.getBool("deskClock", true);
  desktopShowDate = preferences.getBool("deskDate", true);
  desktopShowHint = preferences.getBool("deskHint", true);
  desktopShowReminders = preferences.getBool("deskEvents", true);
  lockEnabled = preferences.getBool("lockEn", false);
  lockTimeoutMinutes = preferences.getUInt("lockMin", 5);
  bool validLockTimeout = false;
  for (uint8_t i = 0; i < LOCK_TIMEOUT_COUNT; i++) {
    if (lockTimeoutMinutes == lockTimeoutOptions[i]) { validLockTimeout = true; break; }
  }
  if (!validLockTimeout) lockTimeoutMinutes = 5;
  systemPin = preferences.getString("sysPin", "1234");
  if (systemPin.length() < 4 || systemPin.length() > 6) systemPin = "1234";
  systemLocked = false;
  lastUserActivity = millis();
  LittleFS.begin(true);
  if (!LittleFS.exists("/events")) LittleFS.mkdir("/events");
  if (!LittleFS.exists("/secure")) LittleFS.mkdir("/secure");
  if (!LittleFS.exists("/contacts")) LittleFS.mkdir("/contacts");
  securePinHash = preferences.getString("secPinHash", "");
  secureVaultUnlocked = false;
  secureSessionPin = "";
  ensureProgramFile();

  SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.fillScreen(COLOR_BG);


  u8g2.begin(tft);
  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);

  keypad.setDebounceTime(30);
  keypad.setHoldTime(1000);

  bootScreen();
  connectSavedWiFi();
  markUserActivity();
  currentScreen = SCREEN_DESKTOP;
  drawDesktop();
}




void loop() {
  updateAutoLock();
  if (currentScreen == SCREEN_PROGRAM_RUN) updateProgramRun();
  if (currentScreen == SCREEN_DESKTOP) updateDesktop();
  if (currentScreen == SCREEN_CLOCK) updateClock();
  if (currentScreen == SCREEN_TIMER) updateTimer();
  if (currentScreen == SCREEN_STOPWATCH) updateStopwatch();
  if (currentScreen == SCREEN_PONG) updatePong();


  if (httpServerRunning) {
    if (WiFi.status() == WL_CONNECTED) httpServer.handleClient();
    else stopHttpServer();
  }

  if (WiFi.status() != WL_CONNECTED && millis() - lastReconnectAttempt > 10000) {
    lastReconnectAttempt = millis();
    String savedSSID = preferences.getString("ssid", "");
    if (savedSSID.length()) WiFi.reconnect();
  }

  if (WiFi.status() == WL_CONNECTED && !ntpConfigured) configureKaliningradTime();

  if (keypad.getKeys()) {
    for (int i = 0; i < LIST_MAX; i++) {
      if (!keypad.key[i].stateChanged) continue;
      char key = keypad.key[i].kchar;
      KeyState state = keypad.key[i].kstate;
      if (state == PRESSED) {
        markUserActivity();
        handlePressedKey(key);
      } else if (state == HOLD) {
        markUserActivity();
        handleHeldKey(key);
      }
    }
  }
}
