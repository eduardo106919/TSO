# Teste Modelo - TSO

## Grupo 1

**1.** Selecione a(s) afirmação(ões) correta(s) sobre monitorização e observabilidade.

- [ ] A monitorização é mais detalhada do que a observabilidade
- [ ] A monitorização ajuda a detetar que algo está errado, enquanto que a observabilidade ajuda a perceber o porquê
- [ ] A observabilidade ajuda a detetar que algo está errado, enquanto que a monitorização ajuda a perceber o porquê
- [ ] A monitorização e a observabilidade são complementares

**2.** Qual ou quais das seguintes afirmações descrevem corretamente o papel do verificador eBPF?

- [ ] Compila o código C em bytecode eBPF
- [ ] Traduz o bytecode eBPF em código máquina nativo
- [ ] Analisa o programa eBPF antes da sua execução para garantir que é seguro para o kernel
- [ ] Carrega o programa eBPF para o _hook_ pretendido via _syscall_

**3.** Selecione a(s) afirmação(ões) que descreve(m) corretamente o impacto do agrupamento de pedidos (_batching_) no desempenho de um sistema.

- [ ] Melhora simultaneamente o débito e a latência
- [ ] Melhora a latência ao custo do débito
- [ ] Melhora o débito ao custo da latência
- [ ] Não tem qualquer impacto no débito nem na latência

**4.** Indique a(s) afirmação(ões) corretas relativamente às diferenças entre as APIs _POSIX_, _libaio_ e _io_uring_.

- [ ] A API POSIX é assíncrona, enquanto que as APIs _libaio_ e _io_uring_ são síncronas
- [ ] A API POSIX é síncrona, enquanto que as APIs _libaio_ e _io_uring_ são assíncronas
- [ ] A API _libaio_ usa _polling_ para recolher o resultado de pedidos de E/S
- [ ] A API _io_uring_ pode usar _polling_ para recolher o resultado de pedidos de E/S

**5.** Escolha a(s) opção(ões) correta(s) sobre escalonamento de dados.

- [ ] O algoritmo _lottery ticketing_ não necessita de aleatoriedade na sua implementação
- [ ] O algoritmo _lottery ticketing_ necessita de aleatoriedade na sua implementação
- [ ] O algoritmo de _stride scheduling_ é probabilisticamente correto
- [ ] O algoritmo de _stride scheduling_ é determinístico

## Grupo 2

**1.** "_A frequência de recolha e o nível de detalhe recolhido por uma ferramenta de coleção de dados de observabilidade podem ter um impacto significativo no consumo de recursos e no desempenho do sistema._" Concorda com esta afirmação? <u>Justifique a sua resposta com exemplos concretos.</u>

**2.** Imagine que pretende contabilizar o número de chamadas ao sistema feitas por uma implementação FUSE _passthrough_. De que forma pode a tecnologia eBPF ser útil? Que componentes/locais da stack de I/O escolheria para fazer _tracing_? <u>Justifique a sua resposta.</u>

**3.** Foi encarregue de implementar um sistema de deduplicação para discos SSD NVMe. Como requisito principal, é-lhe indicado que a sua solução deve manter a baixa latência (_i.e._, tempo de resposta) para pedidos de escrita e leitura que este tipo de discos oferece às aplicações. Tendo em conta este requisito, acha melhor optar por uma solução de deduplicação _inline_ ou _offline_? <u>Justifique a sua resposta</u>, indicando concretamente os pontos fortes ou fracos destes desenhos que justificam a sua escolha.
