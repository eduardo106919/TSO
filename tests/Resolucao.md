# Teste Modelo - TSO

## Grupo 1

**1.** Selecione a(s) afirmação(ões) correta(s) sobre monitorização e observabilidade.

- [ ] A monitorização é mais detalhada do que a observabilidade
- [x] A monitorização ajuda a detetar que algo está errado, enquanto que a observabilidade ajuda a perceber o porquê
- [ ] A observabilidade ajuda a detetar que algo está errado, enquanto que a monitorização ajuda a perceber o porquê
- [x] A monitorização e a observabilidade são complementares

**2.** Qual ou quais das seguintes afirmações descrevem corretamente o papel do verificador eBPF?

- [ ] Compila o código C em bytecode eBPF
- [ ] Traduz o bytecode eBPF em código máquina nativo
- [x] Analisa o programa eBPF antes da sua execução para garantir que é seguro para o kernel
- [x] Carrega o programa eBPF para o _hook_ pretendido via _syscall_

**3.** Selecione a(s) afirmação(ões) que descreve(m) corretamente o impacto do agrupamento de pedidos (_batching_) no desempenho de um sistema.

- [ ] Melhora simultaneamente o débito e a latência
- [ ] Melhora a latência ao custo do débito
- [x] Melhora o débito ao custo da latência
- [ ] Não tem qualquer impacto no débito nem na latência

**4.** Indique a(s) afirmação(ões) corretas relativamente às diferenças entre as APIs _POSIX_, _libaio_ e _io_uring_.

- [ ] A API POSIX é assíncrona, enquanto que as APIs _libaio_ e _io_uring_ são síncronas
- [x] A API POSIX é síncrona, enquanto que as APIs _libaio_ e _io_uring_ são assíncronas
- [ ] A API _libaio_ usa _polling_ para recolher o resultado de pedidos de E/S
- [x] A API _io_uring_ pode usar _polling_ para recolher o resultado de pedidos de E/S

**5.** Escolha a(s) opção(ões) correta(s) sobre escalonamento de dados.

- [ ] O algoritmo _lottery ticketing_ não necessita de aleatoriedade na sua implementação
- [x] O algoritmo _lottery ticketing_ necessita de aleatoriedade na sua implementação
- [ ] O algoritmo de _stride scheduling_ é probabilisticamente correto
- [x] O algoritmo de _stride scheduling_ é determinístico

## Grupo 2

**1.** "_A frequência de recolha e o nível de detalhe recolhido por uma ferramenta de coleção de dados de observabilidade podem ter um impacto significativo no consumo de recursos e no desempenho do sistema._" Concorda com esta afirmação? <u>Justifique a sua resposta com exemplos concretos.</u>

**Resposta:** Sim, a afirmação está correta. Quanto mais frequentemente se recolhem métricas e quanto mais granular é a informação recolhida, maior é o volume de dados gerado e maior é o trabalho que o sistema tem de realizar para os produzir, transportar e armazenar. Por exemplo, um sistema de _logging_ detalhado, como _debug_ ou _trace_, aumenta o volume de escrita em disco e o número de _syscalls_. Em sistemas de alto débito, isto pode degradar o desempenho de forma visível. Um programa eBPF que recolhe eventos de _kernel_ a cada _syscall_ ou a cada pacote de rede pode gerar milhões de eventos por segundo. Se estes eventos forem exportados para _user space_ sem filtragem, o custo de cópia e processamento pode saturar a CPU.

**2.** Imagine que pretende contabilizar o número de chamadas ao sistema feitas por uma implementação FUSE _passthrough_. De que forma pode a tecnologia eBPF ser útil? Que componentes/locais da stack de I/O escolheria para fazer _tracing_? <u>Justifique a sua resposta.</u>

**Resposta:** A tecnologia eBPF é útil porque permite observar o comportamento da _stack_ de I/O sem modificar o código da implementação FUSE, sem reiniciar o sistema e com _overhead_ muito reduzido. O local mais direto para medir chamadas ao sistema é a camada de _syscalls_. Como o FUSE _passthrough_ replica operações do VFS para o sistema de ficheiros subjacente, anexar _tracepoints_ nesta camada reflete exatamente o trabalho adicional introduzido pela camada FUSE.

**3.** Foi encarregue de implementar um sistema de deduplicação para discos SSD NVMe. Como requisito principal, é-lhe indicado que a sua solução deve manter a baixa latência (_i.e._, tempo de resposta) para pedidos de escrita e leitura que este tipo de discos oferece às aplicações. Tendo em conta este requisito, acha melhor optar por uma solução de deduplicação _inline_ ou _offline_? <u>Justifique a sua resposta</u>, indicando concretamente os pontos fortes ou fracos destes desenhos que justificam a sua escolha.

**Resposta:** A melhor escolha para manter a baixa latência típica de SSDs NVMe é deduplicação _offline_. A deduplicação _inline_ introduz trabalho extra no caminho crítico de escrita e leitura, o que aumenta inevitavelmente a latência. A deduplicação _offline_ funciona fora do caminho crítico. Os dados são escritos no SSD imediatamente e o processo de deduplicação ocorre mais tarde, em _background_. Isto preserva o tempo de resposta extremamente baixo dos SSDs NVMe.

Pontos fortes:

- Latência mínima, as escritas vão diretamente para o SSD, sem _hashing_ ou _lookup_ no caminho crítico.
- Aproveita o paralelismo do NVMe, o dispositivo pode realizar I/O enquanto a deduplicação corre noutros núcleos.

Pontos fracos:

- Deduplicação não é imediata.
- Requer espaço temporário para dados ainda não deduplicados.
- Pode haver picos de I/O durante o processo de limpeza.
