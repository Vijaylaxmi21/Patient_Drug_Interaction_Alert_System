#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_SIZE 50
#define CLASS_SIZE 40
#define NOTE_SIZE 100

typedef enum {
    MINOR = 1,
    MAJOR,
    CONTRAINDICATED
} Severity;

typedef struct {
    int id;
    char name[NAME_SIZE];
    char therapeuticClass[CLASS_SIZE];
    char dosageNote[NOTE_SIZE];
} Drug;

typedef struct {
    int drug1;
    int drug2;
    Severity severity;
    char note[NOTE_SIZE];
} Interaction;

typedef struct {
    Drug *drugs;
    int drugCount;
    Interaction *interactions;
    int interactionCount;
} Database;



void readLine(const char *message, char *buffer, int size) {
    printf("%s", message);

    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
}

int readInt(const char *message) {
    char buffer[100];
    char *end;
    long value;

    while (1) {
        readLine(message, buffer, sizeof(buffer));

        value = strtol(buffer, &end, 10);

        while (*end == ' ' || *end == '\t') {
            end++;
        }

        if (buffer[0] != '\0' &&
            *end == '\0' &&
            value >= 0 && value <= 2147483647L) {
            return (int)value;
        }

        printf("Invalid input! Enter a valid number.\n");
    }
}



int findDrug(Database *db, int id) {
    int i;

    for (i = 0; i < db->drugCount; i++) {
        if (db->drugs[i].id == id) {
            return i;
        }
    }

    return -1;
}



Interaction *findInteraction(Database *db, int id1, int id2) {
    int i;

    for (i = 0; i < db->interactionCount; i++) {
        if ((db->interactions[i].drug1 == id1 &&
             db->interactions[i].drug2 == id2) ||
            (db->interactions[i].drug1 == id2 &&
             db->interactions[i].drug2 == id1)) {

            return &db->interactions[i];
        }
    }

    return NULL;
}



void addDrug(Database *db) {
    Drug *temp;
    Drug newDrug;

    newDrug.id = readInt("Enter Drug ID: ");

    if (findDrug(db, newDrug.id) != -1) {
        printf("Drug ID already exists!\n");
        return;
    }

    readLine("Enter Drug Name: ",
             newDrug.name, NAME_SIZE);

    readLine("Enter Therapeutic Class: ",
             newDrug.therapeuticClass, CLASS_SIZE);

    strcpy(newDrug.dosageNote,
           "Demonstration only - no clinical dosage limit.");

    if (strlen(newDrug.name) == 0 ||
        strlen(newDrug.therapeuticClass) == 0) {
        printf("Name and class cannot be empty.\n");
        return;
    }

    temp = realloc(db->drugs,
                   (db->drugCount + 1) * sizeof(Drug));

    if (temp == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    db->drugs = temp;
    db->drugs[db->drugCount] = newDrug;
    db->drugCount++;

    printf("Drug added successfully!\n");
}



void addInteraction(Database *db) {
    Interaction newInteraction;
    Interaction *temp;

    newInteraction.drug1 =
        readInt("Enter First Drug ID: ");

    newInteraction.drug2 =
        readInt("Enter Second Drug ID: ");

    if (newInteraction.drug1 == newInteraction.drug2 ||
        findDrug(db, newInteraction.drug1) == -1 ||
        findDrug(db, newInteraction.drug2) == -1) {
        printf("Invalid or identical drug IDs!\n");
        return;
    }

    if (findInteraction(db, newInteraction.drug1,
                        newInteraction.drug2) != NULL) {
        printf("Interaction already exists!\n");
        return;
    }

    printf("\n1. Minor\n");
    printf("2. Major\n");
    printf("3. Contraindicated\n");

    int choice = readInt("Select Severity: ");

    if (choice < 1 || choice > 3) {
        printf("Invalid severity!\n");
        return;
    }

    newInteraction.severity = (Severity)choice;

    readLine("Enter interaction description: ",
             newInteraction.note, NOTE_SIZE);

    temp = realloc(db->interactions,
                   (db->interactionCount + 1) *
                   sizeof(Interaction));

    if (temp == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    db->interactions = temp;
    db->interactions[db->interactionCount] =
        newInteraction;

    db->interactionCount++;

    printf("Interaction added successfully!\n");
}



void displaySeverity(Severity severity) {
    switch (severity) {
        case MINOR:
            printf("MINOR");
            break;

        case MAJOR:
            printf("MAJOR");
            break;

        case CONTRAINDICATED:
            printf("CONTRAINDICATED");
            break;

        default:
            printf("UNKNOWN");
    }
}



void displayDrugs(Database *db) {
    int i;

    printf("\n========== DRUG DATABASE ==========\n");

    for (i = 0; i < db->drugCount; i++) {
        printf("\nDrug ID: %d\n", db->drugs[i].id);
        printf("Name: %s\n", db->drugs[i].name);
        printf("Class: %s\n",
               db->drugs[i].therapeuticClass);
        printf("Dosage note: %s\n",
               db->drugs[i].dosageNote);
    }

    printf("\nTotal Drugs: %d\n", db->drugCount);
}



void displayInteractionReport(
    Database *db, int id1, int id2) {

    Interaction *interaction =
        findInteraction(db, id1, id2);

    if (interaction != NULL) {
        int index1 = findDrug(db, id1);
        int index2 = findDrug(db, id2);

        printf("\nInteraction Found!\n");

        printf("%s + %s\n",
               db->drugs[index1].name,
               db->drugs[index2].name);

        printf("Severity: ");
        displaySeverity(interaction->severity);

        printf("\nDescription: %s\n",
               interaction->note);
    }
}



void checkInteractions(Database *db) {
    int n, i, j;
    int *activeDrugs;

    n = readInt("\nEnter number of active medicines: ");

    if (n < 1 || n > 1000) {
        printf("Enter a number between 1 and 1000.\n");
        return;
    }

    activeDrugs = malloc(n * sizeof(int));

    if (activeDrugs == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    for (i = 0; i < n; i++) {
        activeDrugs[i] = readInt("Enter Drug ID: ");

        if (findDrug(db, activeDrugs[i]) == -1) {
            printf("Drug ID not found! Try again.\n");
            i--;
            continue;
        }

        for (j = 0; j < i; j++) {
            if (activeDrugs[j] == activeDrugs[i]) {
                printf("Duplicate ID! Enter another ID.\n");
                i--;
                break;
            }
        }
    }

    printf("\n======= INTERACTION REPORT =======\n");

    int found = 0;

    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {

            Interaction *interaction =
                findInteraction(db,
                                activeDrugs[i],
                                activeDrugs[j]);

            if (interaction != NULL) {
                displayInteractionReport(
                    db, activeDrugs[i], activeDrugs[j]);

                found = 1;
            }
        }
    }

    if (!found) {
        printf("No matching interaction is recorded ");
        printf("in the demonstration database.\n");
    }

    printf("\nIMPORTANT: This report is not a ");
    printf("clinical safety clearance.\n");

    free(activeDrugs);
}



int hasConflict(Database *db, int candidate,
                int active[], int count) {
    int i;

    for (i = 0; i < count; i++) {
        if (findInteraction(db, candidate, active[i])
            != NULL) {
            return 1;
        }
    }

    return 0;
}


void suggestAlternatives(Database *db) {
    int targetID, n, i, index;
    int *active;

    targetID = readInt("\nEnter Drug ID to replace: ");

    index = findDrug(db, targetID);

    if (index == -1) {
        printf("Drug not found!\n");
        return;
    }

    n = readInt(
        "Enter number of OTHER active medicines: ");

    if (n < 0 || n > 1000) {
        printf("Invalid number!\n");
        return;
    }

    active = NULL;

    if (n > 0) {
        active = malloc(n * sizeof(int));

        if (active == NULL) {
            printf("Memory allocation failed!\n");
            return;
        }
    }

    for (i = 0; i < n; i++) {
        active[i] = readInt("Enter active Drug ID: ");

        if (findDrug(db, active[i]) == -1 ||
            active[i] == targetID) {
            printf("Invalid ID! Try again.\n");
            i--;
            continue;
        }
    }

    printf("\n======= DEMONSTRATION ALTERNATIVES =======\n");

    int found = 0;

    for (i = 0; i < db->drugCount; i++) {

        if (i == index) {
            continue;
        }

        if (strcmp(db->drugs[i].therapeuticClass,
                   db->drugs[index].therapeuticClass) == 0
            &&
            !hasConflict(db, db->drugs[i].id, active, n)) {

            printf("Candidate: %s (ID: %d)\n",
                   db->drugs[i].name,
                   db->drugs[i].id);

            found = 1;
        }
    }

    if (!found) {
        printf("No matching demonstration candidates found.\n");
    }

    printf("\nThese are educational database matches only, ");
    printf("not medical replacement recommendations.\n");

    free(active);
}



void initializeDatabase(Database *db) {

    db->drugCount = 5;
    db->interactionCount = 3;

    db->drugs = malloc(5 * sizeof(Drug));
    db->interactions = malloc(3 * sizeof(Interaction));

    if (db->drugs == NULL ||
        db->interactions == NULL) {

        printf("Memory allocation failed!\n");
        free(db->drugs);
        free(db->interactions);
        exit(1);
    }

    db->drugs[0] = (Drug){
        101, "DemoMed-A", "Class-Alpha",
        "Demo only - no clinical dosage limit."
    };

    db->drugs[1] = (Drug){
        102, "DemoMed-B", "Class-Alpha",
        "Demo only - no clinical dosage limit."
    };

    db->drugs[2] = (Drug){
        103, "DemoMed-C", "Class-Alpha",
        "Demo only - no clinical dosage limit."
    };

    db->drugs[3] = (Drug){
        104, "DemoMed-D", "Class-Beta",
        "Demo only - no clinical dosage limit."
    };

    db->drugs[4] = (Drug){
        105, "DemoMed-E", "Class-Beta",
        "Demo only - no clinical dosage limit."
    };

    db->interactions[0] = (Interaction){
        101, 102, MAJOR,
        "Sample major interaction for demonstration."
    };

    db->interactions[1] = (Interaction){
        101, 103, MINOR,
        "Sample minor interaction for demonstration."
    };

    db->interactions[2] = (Interaction){
        104, 105, CONTRAINDICATED,
        "Sample contraindicated pair for demonstration."
    };
}


int main(void) {
    Database db;
    int choice;

    initializeDatabase(&db);

    do {
        printf("\n");
        printf("====================================\n");
        printf(" PATIENT DRUG INTERACTION ALERT SYSTEM\n");
        printf("====================================\n");

        printf("1. Display Drug Database\n");
        printf("2. Check Drug Interactions\n");
        printf("3. Suggest Alternatives\n");
        printf("4. Add New Drug\n");
        printf("5. Add New Interaction\n");
        printf("0. Exit\n");

        printf("====================================\n");

        choice = readInt("Enter your choice: ");

        switch (choice) {

            case 1:
                displayDrugs(&db);
                break;

            case 2:
                checkInteractions(&db);
                break;

            case 3:
                suggestAlternatives(&db);
                break;

            case 4:
                addDrug(&db);
                break;

            case 5:
                addInteraction(&db);
                break;

            case 0:
                printf("\nExiting program...\n");
                break;

            default:
                printf("Invalid choice! Try again.\n");
        }

    } while (choice != 0);

    free(db.drugs);
    free(db.interactions);

    db.drugs = NULL;
    db.interactions = NULL;

    printf("Memory released successfully.\n");

    return 0;
}