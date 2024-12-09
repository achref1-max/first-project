#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    int id;                      
    char description[100];       
    struct Task* next;           
} Task;

typedef struct Queue {
    Task* head;                 
    Task* tail;                  
} Queue;

typedef struct stack {
    Task task;                  
    struct stack* next;      
} stack;


Queue taskQueue = {NULL, NULL};
stack* undoStack = NULL;


void addTask();
void viewTasks();
void completeTask();
void undoLastOperation();
void saveTasksToFile();
void loadTasksFromFile();
Task* createTask(int id, const char* description);
void enqueue(Task* newTask);
Task* dequeue();
void pushToStack(Task task);
Task popFromStack();
void menu();


int main() {
    loadTasksFromFile(); 
    menu();              
    saveTasksToFile();   
    return 0;
}


void menu() {
    int choice;
    do {
        printf("\nMenu du gestionnaire de taches:\n");
        printf("1. Ajouter une tache\n");
        printf("2. Afficher les taches\n");
        printf("3. Completer une tache\n");
        printf("4. Annuler la dernière opération\n");
        printf("5. Sortie\n");
        printf("Entrer votre choix: ");
        scanf("%d", &choice);
        getchar(); 

        switch (choice) {
            case 1: addTask(); break;
            case 2: viewTasks(); break;
            case 3: completeTask(); break;
            case 4: undoLastOperation(); break;
            case 5: printf("Sortie...\n"); break;
            default: printf("Choix invalide. Veuillez reessayer.\n");
        }
    } while (choice != 5);
}


Task* createTask(int id, const char* description) {
    Task* newTask = (Task*)malloc(sizeof(Task));
    newTask->id = id;
    strcpy(newTask->description, description);
    newTask->next = NULL;
    return newTask;
}


void addTask() {
    int id;
    char description[100];
    printf("Entrer l'id de la tache: ");
    scanf("%d", &id);
    getchar(); 
    printf("Entrer la description de la tache: ");
    fgets(description, sizeof(description), stdin);
    description[strcspn(description, "\n")] = '\0'; 

    Task* newTask = createTask(id, description);
    enqueue(newTask);
    printf("Tache ajoutee avec succes.\n");
}


void viewTasks() {
    if (taskQueue.head == NULL) {
        printf("Aucune tache disponible.\n");
        return;
    }
    printf("\nTaches:\n");
    Task* current = taskQueue.head;
    while (current != NULL) {
        printf("ID: %d, Description: %s\n", current->id, current->description);
        current = current->next;
    }
}


void completeTask() {
    Task* completedTask = dequeue();
    if (completedTask) {
        pushToStack(*completedTask);
        printf("la tache '%s' est terminee et deplacee vers undo stack.\n", completedTask->description);
        free(completedTask);
    } else {
        printf("Aucune tache a accomplir.\n");
    }
}


void undoLastOperation() {
    if (undoStack == NULL) {
        printf("Rien a defaire.\n");
        return;
    }
    Task lastTask = popFromStack();
    Task* newTask = createTask(lastTask.id, lastTask.description);
    enqueue(newTask);
    printf("tache '%s' restaure a partir de undo stack.\n", lastTask.description);
}


void saveTasksToFile() {
    FILE* file = fopen("tasks.txt", "w");
    if (!file) {
        printf("Erreur lors de l'enregistrement des taches dans le fichier.\n");
        return;
    }
    Task* current = taskQueue.head;
    while (current != NULL) {
        fprintf(file, "%d|%s\n", current->id, current->description);
        current = current->next;
    }
    fclose(file);
    printf("Taches enregistrees dans le fichier.\n");
}



void loadTasksFromFile() {
    FILE* file = fopen("tasks.txt", "r");
    if (!file) {
        printf("Aucune tache trouvee a charger.\n");
        return;
    }
    int id;
    char description[100];
    while (fscanf(file, "%d|%[^\n]\n", &id, description) != EOF) {
    Task* newTask = createTask(id, description);
    enqueue(newTask);
}

    fclose(file);
    printf("Taches chargees a partir du fichier.\n");
}


void enqueue(Task* newTask) {
    if (taskQueue.tail == NULL) {
        taskQueue.head = taskQueue.tail = newTask;
    } else {
        taskQueue.tail->next = newTask;
        taskQueue.tail = newTask;
    }
}


Task* dequeue() {
    if (taskQueue.head == NULL) return NULL;
    Task* temp = taskQueue.head;
    taskQueue.head = taskQueue.head->next;
    if (taskQueue.head == NULL) {
        taskQueue.tail = NULL;
    }
    return temp;
}


void pushToStack(Task task) {
    stack* newNode = (stack*)malloc(sizeof(stack));
    newNode->task = task;
    newNode->next = undoStack;
    undoStack = newNode;
}


Task popFromStack() {
    if (undoStack == NULL) {
        Task emptyTask = {0, "", NULL};
        return emptyTask;
    }
    stack* temp = undoStack;
    Task task = temp->task;
    undoStack = undoStack->next;
    free(temp);
    return task;
}
