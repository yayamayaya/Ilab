#include "main.h"

FILE *akLog = NULL;
const char *format = "%20s|%20d|%20s\n";

#ifdef DEBUG
#define AK_LOG_CLOSE() fclose(akLog)
#else
#define AK_LOG_CLOSE() do {} while (0)
#endif

int startAkinator(node_t *root);
int chooseAction();

int main(const int argc, const char *argv[])
{
#ifdef DEBUG
    akLog = openLogFile(2, argv, "akinatorLog.log", "%20s|%20s|%20s|\n", "MESSAGE", "MESSAGE_NUMBER", "ERROR");
    if (!akLog)
        return LOG_FILE_OPEN_ERR;
#endif

    //Происходит "чтение" файла
    char *buff = NULL;
    int pos = 0;

    readToBuff(DATABASE_LOC, &buff, NULL);
    if (buff == NULL)
    {
        LOG(">> data base has NULL pointer", "");
    }
    
    node_t *root = readDataBase(buff, &pos);
    free(buff);

    int errorMsg = startAkinator(root);
    
    FILE *dataBase = fopen(DATABASE_LOC, "w");
    treePrint(dataBase, root);
    fclose(dataBase);
    treeKill_string(root);
    AK_LOG_CLOSE();

    return 0;
}

int startAkinator(node_t *root)
{
    setbuf(stdout, NULL);
    int akinMsg = 0;

    do
    {
        int option = chooseAction();   
        if (option == END_PROGRAM)
            return END_PROGRAM;
        else if (option == AKINATE)
        {
            akinMsg = Akinator(root);
            if (akinMsg)
                return akinMsg;
        }
        else if (option == DESCRIPTION)
            giveDescription(root);
        else if (option == COMPARE)
            compareTwoObjects(root);
        
    } while (akinatorAsk(">>  Do you want to continue%s", "?"));
    
    if (akinatorAsk(">> Do you want to create a picture if a database%s", "?"))
        system("../akinator/graph_creator/graph");
    

    return FATAL_ERR;
}

int chooseAction()
{
    printf(">>  Choose option:\n");
    printf("1) Run Akinator Game\n");
    printf("2) Find description of an object\n");
    printf("3) Compare two objects\n");

    int option = 0;
    int scanned = 0;
    do
    {
        printf(">>  Type a number beetwen [0-3], where '0' is stop programm:\n");
        scanf("%d%n", &option, &scanned);
    } while (scanned == 0 && !(0 <= option && option <= 3));
    
    return option;
}



