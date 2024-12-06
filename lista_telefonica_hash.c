#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO_TABELA 100

typedef struct Contato {
    char nome[100];
    char telefone[15];
    struct Contato *proximo;
} Contato;

Contato *tabelaHash[TAMANHO_TABELA];

unsigned int hash(char *nome) {
    unsigned int valorHash = 0;
    for (int i = 0; nome[i] != '\0'; i++) {
        valorHash += nome[i];
    }
    return valorHash % TAMANHO_TABELA;
}

void registrarOperacao(const char *operacao, const char *nome, double tempo) {
    FILE *arquivo = fopen("saida.json", "a");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }
    fprintf(arquivo, "{\n");
    fprintf(arquivo, "  \"operacao\": \"%s\",\n", operacao);
    fprintf(arquivo, "  \"nome\": \"%s\",\n", nome);
    fprintf(arquivo, "  \"tempo_ms\": %.2f\n", tempo);
    fprintf(arquivo, "},\n");
    fclose(arquivo);
}

void adicionarContato(char *nome, char *telefone) {
    unsigned int indice = hash(nome);
    Contato *novoContato = (Contato *)malloc(sizeof(Contato));
    strcpy(novoContato->nome, nome);
    strcpy(novoContato->telefone, telefone);
    novoContato->proximo = tabelaHash[indice];
    tabelaHash[indice] = novoContato;
    printf("Contato adicionado: %s - %s\n", nome, telefone);
}

void buscarContato(char *nome) {
    unsigned int indice = hash(nome);
    Contato *atual = tabelaHash[indice];
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            printf("Encontrado: %s - %s\n", atual->nome, atual->telefone);
            return;
        }
        atual = atual->proximo;
    }
    printf("Contato nao encontrado: %s\n", nome);
}

void removerContato(char *nome) {
    unsigned int indice = hash(nome);
    Contato *atual = tabelaHash[indice];
    Contato *anterior = NULL;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            if (anterior == NULL) {
                tabelaHash[indice] = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }
            free(atual);
            printf("Contato removido: %s\n", nome);
            return;
        }
        anterior = atual;
        atual = atual->proximo;
    }
    printf("Contato nao encontrado para remocao: %s\n", nome);
}

void exibirContatos() {
    int contatosEncontrados = 0; 

    for (int i = 0; i < TAMANHO_TABELA; i++) {
        Contato *atual = tabelaHash[i];
        while (atual != NULL) {
            printf("Nome: %s - Telefone: %s\n", atual->nome, atual->telefone);
            atual = atual->proximo;
            contatosEncontrados = 1; 
        }
    }

    if (!contatosEncontrados) {
        printf("Nenhum contato encontrado.\n");
    }
}

int main() {
    int opcao;
    char nome[100], telefone[15];
    clock_t inicio, fim;
    double tempoInsercao = 0, tempoBusca = 0;
    int insercoes = 0, buscas = 0;

    for (int i = 0; i < TAMANHO_TABELA; i++) {
        tabelaHash[i] = NULL;
    }

    FILE *arquivo = fopen("saida.json", "w");
    fprintf(arquivo, "[\n");
    fclose(arquivo);

    do {
        printf("\nEscolha uma opcao:\n");
        printf("1 - Adicionar contato\n");
        printf("2 - Buscar contato por nome\n");
        printf("3 - Remover contato\n");
        printf("4 - Exibir todos os contatos\n");
        printf("5 - Mostrar tempos medios\n");
        printf("0 - Sair\n");
        printf("Digite uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Digite o nome do contato: ");
                scanf(" %[^\n]s", nome);
                printf("Digite o telefone do contato: ");
                scanf(" %[^\n]s", telefone);

                inicio = clock();
                adicionarContato(nome, telefone);
                fim = clock();

                double tempoAtualInsercao = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000;
                registrarOperacao("Adicionar", nome, tempoAtualInsercao);
                tempoInsercao += tempoAtualInsercao;
                insercoes++;
                break;

            case 2:
                printf("Digite o nome do contato para buscar: ");
                scanf(" %[^\n]s", nome);

                inicio = clock();
                buscarContato(nome);
                fim = clock();

                double tempoAtualBusca = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000;
                registrarOperacao("Buscar", nome, tempoAtualBusca);
                tempoBusca += tempoAtualBusca;
                buscas++;
                break;

            case 3:
                printf("Digite o nome do contato para remover: ");
                scanf(" %[^\n]s", nome);
                removerContato(nome);
                break;

            case 4:
                exibirContatos();
                break;

            case 5:
                printf("\nTempo medio de insercao: %.2f ms\n", insercoes > 0 ? tempoInsercao / insercoes : 0);
                printf("Tempo medio de busca: %.2f ms\n", buscas > 0 ? tempoBusca / buscas : 0);
                break;

            case 0:
                printf("Saindo...\n");
                break;

            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 0);

    arquivo = fopen("saida.json", "a");
    fseek(arquivo, -2, SEEK_END); 
    fprintf(arquivo, "\n]\n");
    fclose(arquivo);

    return 0;
}
