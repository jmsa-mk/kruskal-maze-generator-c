#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<windows.h>
#include<conio.h>

#define gotoxy(x, y) printf("\x1b[%d;%dH", (y) + 1, (x) + 1)
#define hideCursor() printf("\x1b[?25l")

struct Map{
    char grid[13][31];
    bool visited[13][31];
    Map* up;
    Map* down;
    Map* left;
    Map* right;
    int id;
    int worldX;
    int worldY;
};

Map* worldMaps[1000]; 
int worldMapCount = 0;

struct Character{
    int x;
    int y;
};

bool walkable(Map* map, int x, int y){
    if(x < 0 || x >= 30 || y < 0 || y >= 12) return false;
    char c = map->grid[y][x];
    if(c == 'י' || c == 'L' || c == 'M') return false;
    return c != '#';
}

void printMap(Map* map, Character* chara){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    gotoxy(0, 0);
    for(int i = 0; i < 13; i++){
        for(int j = 0; j < 31; j++){
            if(i == chara->y && j == chara->x){
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                printf("P");
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
			else if(map->grid[i][j] == '#'){
                printf("%c", 219);
            }
			else if(map->grid[i][j] == 'L'){
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_GREEN | FOREGROUND_INTENSITY);
                printf("L");
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
			else if(map->grid[i][j] == 'M'){
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_RED);
                printf("M");
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
			else if (map->grid[i][j] == 'י'){
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
                printf("י");
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
			else{
                printf("%c", map->grid[i][j]);
            }
        }
        puts("");
    }
    gotoxy(0, 14);
    printf("Room ID: %d\n", map->id);
    printf("World Coordinates: (%d, %d)\n", map->worldX, map->worldY);
}

Map* createMap(int id){
    Map* newMap = (Map*)malloc(sizeof(Map));
    newMap->up = NULL;
    newMap->down = NULL;
    newMap->left = NULL;
    newMap->right = NULL;
    newMap->id = id;

    if(id == 1){
        char home[13][32] = {
            "###############################",
            "#                             #",
            "#                             #",
            "#     ###################     #",
            "#     #       ייי       #     #",
            "#     #                 #     #",
            "#     #    ###   ###    #     #",
            "#     #    #       #    #     #",
            "#     #MMMM#       #LLLL#     #",
            "#     ######       ######     #",
            "#                             #",
            "#                             #",
            "###############################",
        };
        for(int i = 0; i < 13; i++){
            for(int j = 0; j < 31; j++){
                newMap->grid[i][j] = home[i][j];
                newMap->visited[i][j] = false;
            }
        }

        newMap->grid[0][15] = ' ';// atas
        newMap->grid[12][15] = ' ';// bawah
        newMap->grid[6][0] = ' ';// kiri
        newMap->grid[6][30] = ' ';//kanan
    }
	else{
        // Kruskal's Algorithm
        for(int i = 0; i < 13; i++){
            for(int j = 0; j < 31; j++){
                newMap->grid[i][j] = '#';
                newMap->visited[i][j] = false;
            }
        }
        int numCells = (12 / 2) * (30 / 2); // Jumlah sel dalam grid
        int parent[numCells];
        for(int i = 0; i < numCells; i++) parent[i] = i;

        // list semua edge antar sel
        struct Edge{
            int x1, y1, x2, y2;
        };
        Edge edges[1000];
        int edgeCount = 0;
        for(int y = 1; y < 12; y += 2){
            for(int x = 1; x < 30; x += 2){
                newMap->grid[y][x] = ' ';
                if(x + 2 < 30){
                    edges[edgeCount++] = (Edge){x, y, x + 2, y};
                }
                if(y + 2 < 12){
                    edges[edgeCount++] = (Edge){x, y, x, y + 2};
                }
            }
        }

        // Shuffle edges
        srand(time(NULL));
        for(int i = 0; i < edgeCount; i++){
            int j = rand() % edgeCount;
            Edge temp = edges[i];
            edges[i] = edges[j];
            edges[j] = temp;
        }

        // Union-Find
        for(int i = 0; i < edgeCount; i++){
            int cell1 = ((edges[i].y1 - 1) / 2) * (30 / 2) + ((edges[i].x1 - 1) / 2);
            int cell2 = ((edges[i].y2 - 1) / 2) * (30 / 2) + ((edges[i].x2 - 1) / 2);
            while (cell1 != parent[cell1]) cell1 = parent[cell1];
            while (cell2 != parent[cell2]) cell2 = parent[cell2];
            if(cell1 != cell2) {
                parent[cell2] = cell1;
                newMap->grid[(edges[i].y1 + edges[i].y2) / 2][(edges[i].x1 + edges[i].x2) / 2] = ' ';
            }
        }

        newMap->grid[0][15] = ' ';//atas
        newMap->grid[12][15] = ' ';//bawah
        newMap->grid[6][0] = ' ';//kiri
        newMap->grid[6][1] = ' ';//kiri
        newMap->grid[6][30] = ' ';//kanan
        newMap->grid[6][29] = ' ';//kanan
    }
    return newMap;
}

Map* createLinkedMap(Map* from, int dx, int dy){
    Map* m = createMap(from->id + 1);
    m->worldX = from->worldX + dx;
    m->worldY = from->worldY + dy;
    return m;
}

Map* getMapAt(int x, int y){
    for(int i = 0; i < worldMapCount; i++){
        if(worldMaps[i]->worldX == x &&
           worldMaps[i]->worldY == y){
            return worldMaps[i];
        }
    }
    return NULL;
}

Map* getOrCreateMap(int x, int y){
    Map* existing = getMapAt(x, y);
    if(existing) return existing;

    Map* newMap = createMap(worldMapCount + 1);
    newMap->worldX = x;
    newMap->worldY = y;
    worldMaps[worldMapCount++] = newMap;
    return newMap;
}

Map* transition(Map* currentMap, Character* chara){
    if(chara->x == 15 && chara->y <= 0){//atas
        Map* next = getOrCreateMap(
            currentMap->worldX,
            currentMap->worldY - 1
        );
        currentMap->up = next;
        next->down = currentMap;
        chara->y = 11;
        return next;
    }
    if(chara->x == 15 && chara->y >= 12){//bawah
        Map* next = getOrCreateMap(
            currentMap->worldX,
            currentMap->worldY + 1
        );
        currentMap->down = next;
        next->up = currentMap;
        chara->y = 1;
        return next;
    }
    if(chara->x <= 0 && chara->y == 6){//kiri
        Map* next = getOrCreateMap(
            currentMap->worldX - 1,
            currentMap->worldY
        );
        currentMap->left = next;
        next->right = currentMap;
        chara->x = 29;
        return next;
    }
    if(chara->x >= 30 && chara->y == 6){//kanan
        Map* next = getOrCreateMap(
            currentMap->worldX + 1,
            currentMap->worldY
        );
        currentMap->right = next;
        next->left = currentMap;
        chara->x = 1;
        return next;
    }
    return currentMap;
}

void move(Character* chara, char dir){
    switch(dir){
        case 'w': chara->y--; break;
        case 'a': chara->x--; break;
        case 's': chara->y++; break;
        case 'd': chara->x++; break;
    }
}

void game(Map* home){
    Map* currentMap = home;
    Character chara = {15, 7}; // titik start

    while(true){
        printMap(currentMap, &chara);
        hideCursor();
        char dir = _getch();


        int oldX = chara.x;
        int oldY = chara.y;
        move(&chara, dir);
        currentMap->visited[chara.y][chara.x] = true;

        Map* nextMap = transition(currentMap, &chara);

        if(nextMap == currentMap && !walkable(currentMap, chara.x, chara.y)){
            chara.x = oldX;
            chara.y = oldY;
        }
		else{
            currentMap = nextMap;
        }
    }
}


void init(){
    Map* home = createMap(1);
    home->worldX = 0;
    home->worldY = 0;

    worldMaps[worldMapCount++] = home; 

    game(home);
}


int main(){
    init();
    return 0;
}
