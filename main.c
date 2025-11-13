#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



typedef struct {
    int codigo;
    char nome[100];
    char endereco[200];
    char telefone[15];
} Cliente;

typedef struct {
    int codigo;
    char nome[100];
    char telefone[15];
    char cargo[50];
    float salario;
} Funcionario;

typedef struct {
    int numero;
    int quantidadeHospedes;
    float valorDiaria;
    char status[10]; // Status sera "LIVRE" ou "OCUPADO"
} Quarto;

typedef struct {
    int codigo;
    int codigoCliente;
    int numeroQuarto;
    char dataEntrada[11]; // formato "dd/mm/aaaa"
    char dataSaida[11]; // formato "dd/mm/aaaa"
    int quantidadeDiarias;
} Estadia;

/*

Explicaes:
    fseek(file, -sizeof(Cliente), SEEK_END);

    A funao fseek e uma funao da biblioteca padrao de C usada para mover o ponteiro de posiao dentro de um arquivo.

    -sizeof(Cliente): Este e o deslocamento (offset) a partir da posiao especificada por SEEK_END. sizeof(Cliente) retorna o tamanho em bytes da estrutura Cliente.
    O sinal negativo (-) indica que queremos mover o ponteiro para tras, ou seja, retroceder sizeof(Cliente) bytes a partir do final do arquivo.
    SEEK_END: Esta constante indica que o deslocamento deve ser calculado a partir do final do arquivo.

*/

/*

ATENO:
    Ao baixar uma estadia, automaticamente ela  apagada no arquivo Estadia.bin
    Portanto, antes de dar baixa em uma determinada Estadia de um cliente, deve-se calcular os pontos de fidelidade do mesmo

*/

// Cadastrar Quarto [CASE 0]: -------------------------------------------------------------------------------------------------------

void salvarQuarto(Quarto quarto) {
    FILE *file = fopen("quartos.bin", "ab");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de quartos.\n");
        return;
    }
    fwrite(&quarto, sizeof(Quarto), 1, file);
    fclose(file);
}

void cadastrarQuarto() {
    Quarto quarto;
    printf("Digite o numero do quarto: ");
    scanf("%d", &quarto.numero);
    printf("Digite a quantidade de hospedes: ");
    scanf("%d", &quarto.quantidadeHospedes);
    printf("Digite o valor da diaria: ");
    scanf("%f", &quarto.valorDiaria);
    strcpy(quarto.status, "LIVRE");

    // Verificar se o numero do quarto ja existe
    FILE *file = fopen("quartos.bin", "rb");
    if (file != NULL) {
        Quarto quartoExistente;
        while (fread(&quartoExistente, sizeof(Quarto), 1, file)) {
            if (quartoExistente.numero == quarto.numero) {
                printf("Erro: Ja existe um quarto com o numero %d.\n", quarto.numero);
                fclose(file);
                return;
            }
        }
        fclose(file);
    }

    salvarQuarto(quarto);
    printf("Quarto cadastrado com sucesso! Numero do quarto: %d\n", quarto.numero);
}



// Cadastrar Cliente [CASE 1]: ------------------------------------------------------------------------------------------------------

void salvarCliente(Cliente cliente) {
    FILE *file = fopen("clientes.bin", "ab");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de clientes.\n");
        return;
    }
    fwrite(&cliente, sizeof(Cliente), 1, file);
    fclose(file);
}

void cadastrarCliente() {
    Cliente cliente;
    printf("Digite o nome do cliente: ");
    scanf(" %[^\n]", cliente.nome);       //L uma linha de texto (incluindo espaos em branco) ate encontrar o ('\n')
    printf("Digite o endereco do cliente: ");
    scanf(" %[^\n]", cliente.endereco);
    printf("Digite o telefone do cliente: ");
    scanf(" %[^\n]", cliente.telefone);

    FILE *file = fopen("clientes.bin", "rb");

    if (file == NULL) {
        cliente.codigo = 1;
    } else {
        Cliente ultimoCliente;
        fseek(file, -sizeof(Cliente), SEEK_END);
        fread(&ultimoCliente, sizeof(Cliente), 1, file);
        cliente.codigo = ultimoCliente.codigo + 1;
        fclose(file);
    }

    salvarCliente(cliente);
    printf("Cliente cadastrado com sucesso! Codigo do cliente: %d\n", cliente.codigo);
}


// Cadastrar funcionario [CASE 2]: --------------------------------------------------------------------------------------------------

void salvarFuncionario(Funcionario funcionario) {
    FILE *file = fopen("funcionarios.bin", "ab");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de funcionarios.\n");
        return;
    }
    fwrite(&funcionario, sizeof(Funcionario), 1, file);
    fclose(file);
}

void cadastrarFuncionario() {
    Funcionario funcionario;
    printf("Digite o nome do funcionario: ");
    scanf(" %[^\n]", funcionario.nome);
    printf("Digite o telefone do funcionario: ");
    scanf(" %[^\n]", funcionario.telefone);
    printf("Digite o cargo do funcionario: ");
    scanf(" %[^\n]", funcionario.cargo);
    printf("Digite o salario do funcionario: ");
    scanf("%f", &funcionario.salario);

    FILE *file = fopen("funcionarios.bin", "rb");

    if (file == NULL) {
        funcionario.codigo = 1;
    }

    else {
        Funcionario ultimoFuncionario;
        fseek(file, -sizeof(Funcionario), SEEK_END);
        fread(&ultimoFuncionario, sizeof(Funcionario), 1, file);
        funcionario.codigo = ultimoFuncionario.codigo + 1;
        fclose(file);
    }

    salvarFuncionario(funcionario);
    printf("Funcionario cadastrado com sucesso! Codigo do funcionario: %d\n", funcionario.codigo);
}

// Cadastrar Estadia [CASE 3]: ------------------------------------------------------------------------------------------------------

void salvarEstadia(Estadia estadia) {
    FILE *file = fopen("estadias.bin", "ab");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de estadias.\n");
        return;
    }
    fwrite(&estadia, sizeof(Estadia), 1, file);
    fclose(file);
}

// Funao personalizada para converter uma data no formato "dd/mm/aaaa" para um objeto struct tm
void parseDate(char *data, struct tm *tm) {
    sscanf(data, "%d/%d/%d", &tm->tm_mday, &tm->tm_mon, &tm->tm_year);
    tm->tm_mon -= 1;  // Meses em struct tm vao de 0 a 11
    tm->tm_year -= 1900;  // Anos em struct tm sao contados a partir de 1900
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;
    tm->tm_isdst = -1;  // Deixe o sistema determinar se e horario de verao
}

// Funao para converter uma data no formato "dd/mm/aaaa" para um objeto time_t
time_t converterData(char *data) {
    struct tm tm = {0};
    parseDate(data, &tm);
    return mktime(&tm);
}

// Funao para calcular a quantidade de diarias
int calcularDiarias(char *dataEntrada, char *dataSaida) {
    time_t entrada = converterData(dataEntrada);
    time_t saida = converterData(dataSaida);
    double diferenca = difftime(saida, entrada) / (60 * 60 * 24);
    return (int)diferenca;
}

void cadastrarEstadia() {
    Estadia estadia;
    printf("Digite o codigo do cliente: ");
    scanf("%d", &estadia.codigoCliente);
    printf("Digite a quantidade de hospedes: ");
    int quantidadeHospedes;
    scanf("%d", &quantidadeHospedes);
    printf("Digite a data de entrada (dd/mm/aaaa): ");
    scanf("%s", estadia.dataEntrada);
    printf("Digite a data de saida (dd/mm/aaaa): ");
    scanf("%s", estadia.dataSaida);

    // Verificar se o cliente existe
    FILE *clientesFile = fopen("clientes.bin", "rb");
    if (clientesFile == NULL) {
        printf("Erro ao abrir o arquivo de clientes.\n");
        return;
    }
    Cliente cliente;
    int clienteEncontrado = 0;
    while (fread(&cliente, sizeof(Cliente), 1, clientesFile)) {
        if (cliente.codigo == estadia.codigoCliente) {
            clienteEncontrado = 1;
            break;
        }
    }
    fclose(clientesFile);

    if (!clienteEncontrado) {
        printf("Cliente com codigo %d nao encontrado.\n", estadia.codigoCliente);
        return;
    }

    // Encontrar um quarto disponvel
    FILE *quartosFile = fopen("quartos.bin", "rb+");
    if (quartosFile == NULL) {
        printf("Erro ao abrir o arquivo de quartos.\n");
        return;
    }

    Quarto quarto;
    int quartoEncontrado = 0;
    while (fread(&quarto, sizeof(Quarto), 1, quartosFile)) {
        if (quarto.quantidadeHospedes >= quantidadeHospedes && strcmp(quarto.status, "LIVRE") == 0) { // Verifica a qnt. hospedes e status do quarto
            quartoEncontrado = 1;
            strcpy(quarto.status, "OCUPADO");
            fseek(quartosFile, -sizeof(Quarto), SEEK_CUR);
            fwrite(&quarto, sizeof(Quarto), 1, quartosFile);
            estadia.numeroQuarto = quarto.numero;
            break;
        }
    }
    fclose(quartosFile);

    if (!quartoEncontrado) {
        printf("Nao ha quartos disponveis para a quantidade de hospedes especificada.\n");
        return;
    }

    // Calcular a quantidade de diarias
    estadia.quantidadeDiarias = calcularDiarias(estadia.dataEntrada, estadia.dataSaida);

    // Determinar o codigo da nova estadia
    FILE *estadiasFile = fopen("estadias.bin", "rb");
    if (estadiasFile == NULL) {
        estadia.codigo = 1;
    } else {
        Estadia ultimaEstadia;
        fseek(estadiasFile, -sizeof(Estadia), SEEK_END);
        fread(&ultimaEstadia, sizeof(Estadia), 1, estadiasFile);
        estadia.codigo = ultimaEstadia.codigo + 1;
        fclose(estadiasFile);
    }

    salvarEstadia(estadia);
    printf("Estadia cadastrada com sucesso! Codigo da estadia: %d, Numero do quarto: %d\n", estadia.codigo, estadia.numeroQuarto);
}

// Baixa Estadia [CASE 4]: ----------------------------------------------------------------------------------------------------------

int encontrarEstadiaPorCliente(int codigoCliente, Estadia *estadiaEncontrada) {
    FILE *file = fopen("estadias.bin", "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de estadias.\n");
        return 0;
    }
    Estadia estadia;
    while (fread(&estadia, sizeof(Estadia), 1, file)) {
        if (estadia.codigoCliente == codigoCliente) {
            *estadiaEncontrada = estadia;
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void atualizarStatusQuarto(int numeroQuarto, const char *novoStatus) {
    FILE *file = fopen("quartos.bin", "rb+");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de quartos.\n");
        return;
    }
    Quarto quarto;
    while (fread(&quarto, sizeof(Quarto), 1, file)) {
        if (quarto.numero == numeroQuarto) {
            strcpy(quarto.status, novoStatus);
            fseek(file, -sizeof(Quarto), SEEK_CUR);
            fwrite(&quarto, sizeof(Quarto), 1, file);
            fclose(file);
            return;
        }
    }
    fclose(file);
}

void darBaixaEstadia(int codigoCliente) {
    Estadia estadia;
    if (!encontrarEstadiaPorCliente(codigoCliente, &estadia)) {
        printf("Estadia nao encontrada para o cliente com codigo %d.\n", codigoCliente);
        return;
    }

    // Encontrar o quarto
    FILE *quartosFile = fopen("quartos.bin", "rb");
    if (quartosFile == NULL) {
        printf("Erro ao abrir o arquivo de quartos.\n");
        return;
    }
    Quarto quarto;
    int quartoEncontrado = 0;
    while (fread(&quarto, sizeof(Quarto), 1, quartosFile)) {
        if (quarto.numero == estadia.numeroQuarto) {
            quartoEncontrado = 1;
            break;
        }
    }
    fclose(quartosFile);

    if (!quartoEncontrado) {
        printf("Quarto com numero %d nao encontrado.\n", estadia.numeroQuarto);
        return;
    }

    // Calcular o valor total
    float valorTotal = estadia.quantidadeDiarias * quarto.valorDiaria;
    printf("O valor total a ser pago pelo cliente com codigo %d e: R$%.2f\n", codigoCliente, valorTotal);

    // Atualizar o status do quarto para "OCUPADO"
    atualizarStatusQuarto(estadia.numeroQuarto, "LIVRE");

    // Remover ou marcar a estadia como concluda
    FILE *estadiasFile = fopen("estadias.bin", "rb+");
    if (estadiasFile == NULL) {
        printf("Erro ao abrir o arquivo de estadias.\n");
        return;
    }
    FILE *tempFile = fopen("temp.bin", "wb");
    if (tempFile == NULL) {
        printf("Erro ao abrir o arquivo temporario.\n");
        fclose(estadiasFile);
        return;
    }
    Estadia tempEstadia;
    while (fread(&tempEstadia, sizeof(Estadia), 1, estadiasFile)) {
        if (tempEstadia.codigo != estadia.codigo) {
            fwrite(&tempEstadia, sizeof(Estadia), 1, tempFile);
        }
    }
    fclose(estadiasFile);
    fclose(tempFile);

    remove("estadias.bin");
    rename("temp.bin", "estadias.bin");

    printf("Estadia finalizada com sucesso!\n");
}

// Pesquisa Funcionario [CASE 5]: ---------------------------------------------------------------------------------------------------

void mostrarFuncionarioPorCodigo() {
    int codigoProcurado;
    printf("Digite o codigo do funcionario: ");
    scanf("%d", &codigoProcurado);

    FILE *file = fopen("funcionarios.bin", "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de funcionarios.\n");
        return;
    }

    Funcionario funcionario;
    int encontrado = 0;

    while (fread(&funcionario, sizeof(Funcionario), 1, file)) {
        if (funcionario.codigo == codigoProcurado) {
            printf("Funcionario Encontrado:\n");
            printf("Codigo: %d\n", funcionario.codigo);
            printf("Nome: %s\n", funcionario.nome);
            printf("Telefone: %s\n", funcionario.telefone);
            printf("Cargo: %s\n", funcionario.cargo);
            printf("Salario: %.2f\n", funcionario.salario);
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Funcionario com o codigo %d nao encontrado.\n", codigoProcurado);
    }

    fclose(file);
}

// Pesquisa Cliente [CASE 6]: --------------------------------------------------------------------------------------------------

void mostrarClientePorCodigo() {
    int codigoProcurado;
    printf("Digite o codigo do cliente: ");
    scanf("%d", &codigoProcurado);

    FILE *file = fopen("clientes.bin", "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de clientes.\n");
        return;
    }

    Cliente cliente;
    int encontrado = 0;

    while (fread(&cliente, sizeof(Cliente), 1, file)) {
        if (cliente.codigo == codigoProcurado) {
            printf("Cliente Encontrado:\n");
            printf("Codigo: %d\n", cliente.codigo);
            printf("Nome: %s\n", cliente.nome);
            printf("Endereco: %s\n", cliente.endereco);
            printf("Telefone: %s\n", cliente.telefone);
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Cliente com o codigo %d nao encontrado.\n", codigoProcurado);
    }

    fclose(file);
}


// ***** NOVA FUNÇÃO INSERIDA AQUI *****
// Listar Todos os Clientes [CASE 9]: ---------------------------------------------------------------------------------------------

void mostrarTodosClientes() {
    FILE *file = fopen("clientes.bin", "rb"); // Abre o arquivo de clientes
    if (file == NULL) {
        printf("Nao foi possivel abrir o arquivo 'clientes.bin'.\n");
        printf("Verifique se o arquivo existe ou se ja foi cadastrado algum cliente.\n");
        return;
    }

    Cliente cliente; // Variável para armazenar cada cliente lido
    int encontrado = 0; // Flag para saber se pelo menos 1 cliente foi lido

    printf("\n--- Todos os Clientes Cadastrados ---\n");

    // Loop que continua enquanto 'fread' conseguir ler 1 'struct Cliente'
    while (fread(&cliente, sizeof(Cliente), 1, file) == 1) {
        encontrado = 1; // Marca que pelo menos um foi encontrado
        
        // Imprime os dados do cliente lido
        printf("Codigo:   %d\n", cliente.codigo);
        printf("Nome:     %s\n", cliente.nome);
        printf("Endereco: %s\n", cliente.endereco);
        printf("Telefone: %s\n", cliente.telefone);
        printf("---------------------------------------\n");
    }

    if (encontrado == 0) {
        printf("Nenhum cliente cadastrado no arquivo.\n");
    }

    fclose(file); // Fecha o arquivo
}


// Pesquisa Estadia por Cliente[CASE 7]: ---------------------------------------------------------------------------------------

void mostrarEstadiasCliente(int codigoCliente) {
    FILE *file = fopen("estadias.bin", "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de estadias.\n");
        return;
    }

    Estadia estadia;
    int encontradas = 0;

    while (fread(&estadia, sizeof(Estadia), 1, file)) {
        if (estadia.codigoCliente == codigoCliente) {
            printf("Estadia Encontrada:\n");
            printf("Codigo da Estadia: %d\n", estadia.codigo);
            printf("Numero do Quarto: %d\n", estadia.numeroQuarto);
            printf("Data de Entrada: %s\n", estadia.dataEntrada);
            printf("Data de Saida: %s\n", estadia.dataSaida);
            printf("Quantidade de Diarias: %d\n", estadia.quantidadeDiarias);
            printf("------------------------\n");
            encontradas++;
        }
    }

    if (encontradas == 0) {
        printf("Nenhuma estadia encontrada para o cliente com cdigo %d.\n", codigoCliente);
    }

    fclose(file);
}
// Pontos de Fidelidade[CASE 8]: -----------------------------------------------------------------------------------------------

int calcularPontosFidelidade(int codigoCliente) {
    FILE *file = fopen("estadias.bin", "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de estadias.\n");
        return 0;
    }

    Estadia estadia;
    int pontos = 0;

    while (fread(&estadia, sizeof(Estadia), 1, file)) {
        if (estadia.codigoCliente == codigoCliente) {
            pontos += estadia.quantidadeDiarias * 10;
        }
    }

    fclose(file);
    return pontos;
}


// Main ------------------------------------------------------------------------------------------------------------------------

int main() {

    int opcao;

    // ***** MENU ATUALIZADO *****
    do {
        printf("\n _______________________________\n");
        printf("|                               |\n");
        printf("| Menu:                         |\n");
        printf("| 0. Cadastrar Quarto           |\n");
        printf("| 1. Cadastrar Cliente          |\n");
        printf("| 2. Cadastrar Funcionario      |\n");
        printf("| 3. Cadastrar Estadia          |\n");
        printf("| 4. Baixa Estadia              |\n");
        printf("| 5. Pesquisa Funcionario       |\n");
        printf("| 6. Pesquisa Cliente           |\n");
        printf("| 7. Mostrar Estadia do Cliente |\n");
        printf("| 8. Pontos de fidelidade       |\n");
        printf("| 9. Listar Todos os Clientes   |\n"); // <-- NOVA OPÇÃO
        printf("| 10. Sair                      |\n"); // <-- OPÇÃO ANTIGA "9"
        printf("|_______________________________|\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        // ***** SWITCH ATUALIZADO *****
        switch (opcao) {
            case 0:
                cadastrarQuarto();
                break;

            case 1:
                cadastrarCliente();
                break;

            case 2:
                cadastrarFuncionario();
                break;

            case 3:
                cadastrarEstadia();
                break;

            case 4: {
                int codigoCliente;
                printf("Atencao! Verifique os pontos de fidelidade do Cliente antes de dar baixa em sua estadia");
                printf("Digite o codigo do cliente para dar baixa na estadia: ");
                scanf("%d", &codigoCliente);
                darBaixaEstadia(codigoCliente);
                break;
            }

            case 5:
                mostrarFuncionarioPorCodigo();
                break;

            case 6:
                mostrarClientePorCodigo();
                break;

            case 7: {
                int codigoCliente;
                printf("Digite o codigo do cliente: ");
                scanf("%d", &codigoCliente);
                mostrarEstadiasCliente(codigoCliente);
                break;
            }

            case 8:{
                int codigoCliente;
                printf("Digite o codigo do cliente: ");
                scanf("%d", &codigoCliente);
                int pontosFidelidade = calcularPontosFidelidade(codigoCliente);
                printf("Pontos de fidelidade do cliente (codigo %d): %d\n", codigoCliente, pontosFidelidade);
                break;
            }
            
            // ***** NOVO CASE 9 *****
            case 9:
                mostrarTodosClientes();
                break;

            // ***** CASE 10 (ANTIGO 9) *****
            case 10:
                printf("Saindo...\n");
                exit(0);
                break;

            default:
                printf("Opcao invalida.\n");

        }
    // ***** LOOP CORRIGIDO (ERA "opcao != 4") *****
    } while (opcao != 10); 

    return 0;
}