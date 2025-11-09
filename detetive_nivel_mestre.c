#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define TAM_HASH 17
// ------------------- Estruturas -------------------
//representa um cômodo da mansão
typedef struct Sala {
    char nome[50];            // Nome da sala
    char pista[100];          // Pista da sala
    
    struct Sala* esquerda;    // Ponteiro para a sala da esquerda
    struct Sala* direita;     // Ponteiro para a sala da direita
} Sala;

//arvore para registrar todas as pistas que o jogador já coletou
typedef struct Pista {
    char descricao[100];   // Descrição da pista (ex:"Um frasco de veneno vazio")
    struct Pista* esquerda; // Subarvore com pistas "menores" (alfabeticamente)
    struct Pista* direita;  // Subárvore com pistas "maiores" (alfabeticamente)
} Pista;

//nó da tabela hash que associa pistas a suspeitos
typedef struct Node {
    char pista[100];          // Pista associada
    char suspeito[50];        // Nome do suspeito
    struct Node* prox;        // proximo nó da lista encadeada para tratar colisões
    int contador;             // contador de vezes que a pista foi coletada pelo jogador
} Node;

//vriavel global que representa a tabela hash de pistas associadas a suspeitos
Node* tabela_hash[TAM_HASH];


// ------------------- Funções Sala -------------------
//FUNCAO PARA CRIAR UM NOVO COMODO NA MANSAO
Sala* criarSala(const char* nome, const char* pista) {
    //alocacao de memoria para uma nova sala
    Sala* nova = (Sala*)malloc(sizeof(Sala));
    //encerra se a alocacao falhar
    if (nova == NULL) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    //copia o nome do ccomodo para a nova sala
    strcpy(nova->nome, nome);

    //inicializa os ponteiros para salas adjacentes como NULL
    nova->esquerda = NULL;
    nova->direita = NULL;
    //se houver uma pista, copia para o campo pista da sala;
    if (pista != NULL) {
        strcpy(nova->pista, pista);
    } else {
        //se nao houver passa uma pista com string vazia
        strcpy(nova->pista, ""); 
    }
    //retorna o ponteiro para a nova sala criada
    return nova;
}

// ------------------- Funções Pista (BST) -------------------
Pista* inserir_Pista(Pista* raiz, const char* descricao){
    //se a arvore ou a subarvore estiver vazia, a funcao vai um novo no para a pista.
    if (raiz == NULL) {
        Pista* nova = (Pista*)malloc(sizeof(Pista));
        strcpy(nova->descricao, descricao);
        nova->esquerda = NULL;
        nova->direita = NULL;
        return nova;
    }
    //compara a descricao recebida com a descricao da pista atual para evitar duplicidade, caso passe novamente na sala
    int comp = strcmp(descricao, raiz->descricao);
    //se a pista ja existe, nao insere novamente
    if (comp == 0) {
        return raiz;
    //coloca a pista em ordem alfabetica
    } else if (comp < 0) {
        raiz->esquerda = inserir_Pista(raiz->esquerda, descricao);
    } else {
        raiz->direita = inserir_Pista(raiz->direita, descricao);
    }

    return raiz;
}

void Percorrer_Pista(Pista* raiz) {
    //se a arvore ou a subarvore estiver vazia, não percorre nada
    if (raiz == NULL) {
        return;
    }

    //percorre a subavore da esquerda visando a ordem alfabetica
    Percorrer_Pista(raiz->esquerda);

    //depois,mostra a pista atual
    printf("Pista encontrada: %s\n", raiz->descricao);

    //e no final percorre a subárvore da direita
    Percorrer_Pista(raiz->direita);
}

// --------------- Hash ----------------
void inicializarTabela(Node* tabela[]) {
    //percorre a tabela para atribuir NULL aos valores
    for (int i = 0; i < TAM_HASH; i++) {
        tabela[i] = NULL;
    }
}

//calcula um indice com base na soma dos caracteres da chave
int hash(const char *chave) {
    int soma = 0;
    //percorre cada caractere da string até o final
    for(int i = 0; chave[i] != '\0'; i++) {
        soma += chave[i]; //soma o valor ASCII de cada caractere
    }
    //calcula o indice dentro dos limites da tabela
    return soma % TAM_HASH;
}

//imsere uma nova associação entre suspeito e pista na tabela hash (encadeamento)
void inserir_chaining(Node* tabela[], const char* suspeito, const char* pista) {
    // chama a função para calcular o indice onde a pista sera armazenada
    int indice = hash(pista);
    
    //cria um nó para armazenar os dados
    Node* novo = (Node*)malloc(sizeof(Node));
    
    //encerra se a alocacao falhar
    if (novo == NULL) {
        printf("Erro ao alocar memória!\n");
        return;
    }
    
    //copia os valores da pista e do suspeito para o novo nó 
    strcpy(novo->suspeito, suspeito);
    strcpy(novo->pista, pista);
    
    //contar quantas pista está associada a esse suspeito
    novo->contador = 0; 
    
    // Insere no início da lista
    novo->prox = tabela[indice];
    tabela[indice] = novo;
}

//busca uma pista na tabela hash e retorna o nó associado
Node* pista_associada(Node* tabela[], const char* pista) {
    //calcula o indice da tabela hash com base na pista
    int indice = hash(pista);
    
    //ponteiro para percorrer a lista encadeada na posição calculada
    Node* atual = tabela[indice];
    
    //pecoorre a lista enquanto houver nós
    while (atual != NULL) {
    //compara a pista do nó atual com a pista procurada
        if (strcmp(atual->pista, pista) == 0) {
            return atual; //se a pista for encontrada
        }
        //vai para o proximo nó da lista
        atual = atual->prox;
    }
    return NULL; //Se chegar ao final sem achar, retorna NULL
}
//percorre a arvore de pistas em ordem alfabética (bst) e exibe o suspeito associado a cada pista usando a tabela hash
void encontrar_suspeito(Pista* raiz, Node* tabela[]) {
    if (raiz == NULL) return;

    //percorre esquerda primeiro
    encontrar_suspeito(raiz->esquerda, tabela);
 // mudar o tipo exibição aqui vitooooooooo
    //procura o suspeito associado a pista atual
    Node* acusado = pista_associada(tabela, raiz->descricao);
    printf("\nPista: \"%s\", Suspeito: %s\n", raiz->descricao, acusado->suspeito);
    //por ultimo percorre a direita
    encontrar_suspeito(raiz->direita, tabela);
}
//Conta o numero total de pistas encontradas de um suspeito
int contar_pistas_do_suspeito(Node* tabela[], const char* suspeito) {
    //inicializa o contador de pistas
    int total_pistas = 0;
    
    //percorre todos os indices da tabela hash
    for (int i = 0; i < TAM_HASH; i++) {
        //pega o inicio da lista encadeada nesse indice
        Node* atual = tabela[i];
        
        // percorre a lista encadeada do índice atual
        while (atual != NULL) {
            // e o suspeito desse nó for o suspeito procurado
            if (strcmp(atual->suspeito, suspeito) == 0) {
                total_pistas += atual->contador;
            }
            //passa para o proximo nó da lista
            atual = atual->prox;
        }
    }
    return total_pistas; //retorna o total de pistas encontradas
}

void acusar_suspeito(Node* tabela[]) {
    //mostrar o menu de acusacao
    printf("\n====== ACUSAR SUSPEITO ======\n\n");
    printf("1. Maria\n");
    printf("2. Pedro\n\n");
    printf("Escolha o suspeito (digite o número): ");

    int opcao;
    scanf("%d", &opcao); // le a opcao fo usuario
    getchar(); // limpa o buffer

    char suspeito[50];
    //associa o numero da opção ao nome do suspeito
    if (opcao == 1)
        strcpy(suspeito, "Maria");
    else if (opcao == 2)
        strcpy(suspeito, "Pedro");
    else {
        printf("Opção inválida!\n");
        return;
    }
    //calcula o total de pistas associadas ao suspeito escolhido
    int total = contar_pistas_do_suspeito(tabela, suspeito);
    //mostra o resultado da acusacao
    printf("\nAnalisando as evidências...\n");
    if (total >= 2){
        printf("Acusação correta! O suspeito %s tem %d pistas contra ele.\n\n", suspeito, total);
        exit(0);  //encerra o programa imediatamente
    }else
        printf("Não há pistas suficientes para acusar %s (%d encontradas).\n", suspeito, total);
    
}

// ------------------- Exploração -------------------
void explorar_sala(Sala* sala_atual, Pista** pista_atual, Node* tabela[]){
    char escolha;
    int caminho = 0;
    Sala* salas_visitadas[20]; //array para registrar as salas visitadas
    salas_visitadas[caminho++] = sala_atual; // registra o hall como primeira sala

    do {
        printf("\n======= EXPLORANDO A MANSÃO ========\n");
        printf("\nCômodo atual: %s\n", sala_atual->nome);

        // Se houver pista, mostra e adiciona na arvore pista
        if (strlen(sala_atual->pista) > 0) {
            printf("\nVocê encontrou uma pista: %s\n", sala_atual->pista);
    
            //adiciona na árvore BST de pistas coletadas
            *pista_atual = inserir_Pista(*pista_atual, sala_atual->pista);

            //busca a pista na tabela hash para associá-la ao suspeito
            Node* associada = pista_associada(tabela, sala_atual->pista);
            //incrementa contador apenas se a pista ainda não foi associada
            if (associada != NULL && associada->contador == 0) {
                associada->contador++;
            }
        } else {
            printf("\nNenhuma pista encontrada neste cômodo.\n\n");
        }

        // Verifica se é uma folha
        if (sala_atual->esquerda == NULL && sala_atual->direita == NULL) {
            printf("\nEsta sala não possui mais saídas.\n");
        }

        // Pergunta para onde ir
        printf("====================================\n");
        printf("\nDigite a letra (e) para esquerda ou (d) para direita ou (s) para sair: ");
        scanf(" %c", &escolha);
        escolha = tolower(escolha); // converte tudo para minúsculo
        
        //navegação para a esquerda
        if (escolha == 'e') {
            if (sala_atual->esquerda != NULL) {
                sala_atual = sala_atual->esquerda;
                salas_visitadas[caminho++] = sala_atual;
            } else {
                printf("\nNão há sala à esquerda!\n");
            }
        }
        //navegação para a direita
        else if (escolha == 'd') {
            if (sala_atual->direita != NULL) {
                sala_atual = sala_atual->direita;
                salas_visitadas[caminho++] = sala_atual;
            } else {
                printf("\nNão há sala à direita!\n");
            }
        }
        // Sair da exploração
        else if (escolha == 's') {
            printf("\n==========================\n");
            printf("\nVoltando para o Hall...\n");
            break;
        } else {
            printf("Escolha inválida! Digite 'e', 'd' ou 's'.\n");
        }

    } while (1);
    printf("\n==========================\n");
    //exibe as salas visitadas durante a exploração 
    printf("\n====== Salas visitadas ======\n\n");
    for (int i = 0; i < caminho; i++) {
        printf("---%s\n", salas_visitadas[i]->nome);
    }
    //Exibe as pistas coletadas em ordem alfabetica
    printf("\n===== PISTAS COLETADAS =====\n");
    if (*pista_atual == NULL)
        printf("Nenhuma pista coletada\n");
    else
        Percorrer_Pista(*pista_atual);
    printf("============================\n");
}
//========= liberar memória =========
void liberarSalas(Sala* raiz) {
    if (raiz == NULL) return;
    liberarSalas(raiz->esquerda);
    liberarSalas(raiz->direita);
    free(raiz);
}
// libera memória da árvore de pistas
void liberarPistas(Pista* raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}
// libera memória da tabela hash (lista encadeada)
void liberarTabelaHash(Node* tabela[]) {
    for (int i = 0; i < TAM_HASH; i++) {
        Node* atual = tabela[i];
        while (atual != NULL) {
            Node* temp = atual;
            atual = atual->prox;
            free(temp);
        }
        tabela[i] = NULL; //deixa ponteiro nulo
    }
}

int main() {
    //inicializa a tabela hash
    inicializarTabela(tabela_hash);
    
    //associada as pistas aos suspeitos na tabela hash
    inserir_chaining(tabela_hash, "Maria", "Uma carta rasgada com ameaça");
    inserir_chaining(tabela_hash, "Pedro", "Um par de sapatos de tamanho 41");
    inserir_chaining(tabela_hash, "Pedro", "Um frasco de veneno vazio");
    inserir_chaining(tabela_hash, "Pedro", "Pegadas na lama de tamanho 41");

    // criação manual e fixa do mapa da mansão
    Sala* raiz = criarSala("Hall de Entrada", NULL);
    Sala* salaEstar = criarSala("Sala de Estar", "Um par de sapatos de tamanho 41");
    Sala* biblioteca = criarSala("Biblioteca", NULL);
    Sala* cozinha = criarSala("Cozinha", NULL);
    Sala* quarto = criarSala("Quarto", "Um frasco de veneno vazio");
    Sala* salaJantar = criarSala("Sala de Jantar", NULL);
    Sala* jardim = criarSala("Jardim", "Pegadas na lama de tamanho 41");
    Sala* porao = criarSala("Porão", NULL);
    Sala* sotao = criarSala("Sótão", "Uma carta rasgada com ameaça");
    Sala* escritorio = criarSala("Escritório", NULL);

    // Ligações fixas entre salas
    raiz->esquerda = salaEstar;     // Hall de Entrada-> Sala de Estar(pista)
    raiz->direita = biblioteca;     // Hall de Entrada-> Biblioteca

    salaEstar->esquerda = cozinha;  // Sala de Estar-> Cozinha
    salaEstar->direita = quarto;    // Sala de Estar-> Quarto(pista)

    biblioteca->esquerda = salaJantar; // Biblioteca-> Sala de Jantar
    biblioteca->direita = jardim;      // Biblioteca-> Jardim(pista)

    cozinha->esquerda = porao;       // Cozinha-> Porão
    quarto->direita = sotao;         // Quarto-> Sótão(pista)

    salaJantar->esquerda = escritorio; // Sala de Jantar-> Escritório

    //variáveis do menu
    Sala* atual = raiz; //começa dk hall
    Pista* pistasColetadas = NULL; // começa sem pista
    int opcao;

    do {
        printf("\n======= MENU =======\n");
        printf("1. Explorar a mansão\n");
        printf("2. Mostrar suspeitos das pistas coletadas\n");
        printf("3. Acusar algum suspeito\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar(); // limpa o buffer

        switch (opcao) {
            case 1:
                explorar_sala(atual, &pistasColetadas, tabela_hash);
                break;
            case 2:
                if (pistasColetadas != NULL)
                    encontrar_suspeito(pistasColetadas, tabela_hash);
                else
                    printf("Nenhuma pista coletada ainda.\n");
                break;
            case 3:
                acusar_suspeito(tabela_hash);
                break;
            case 0:
                printf("\nSaindo do jogo...\n");
                break;
            default:
                printf("\nOpção inválida!\n");
        }
    } while (opcao != 0);
    
    //Libera memória
    liberarSalas(raiz);
    liberarPistas(pistasColetadas);
    liberarTabelaHash(tabela_hash);

    return 0;
}