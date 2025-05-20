4. Clique em **OK**.
5. Vá em **Ferramentas > Placa > Gerenciador de Placas**.
6. Pesquise por `esp32` e instale o pacote **ESP32 by Espressif Systems**. Isso pode levar alguns minutos.

### 3. Instalar as Bibliotecas Necessárias
O projeto usa algumas bibliotecas que precisam ser instaladas na Arduino IDE:

1. Vá em **Ferramentas > Gerenmentador de Bibliotecas**.
2. Pesquise e instale as seguintes bibliotecas:
- **MPU9250_asukiaaa** (para o sensor MPU9250).
- **ESPAsyncWebServer** (para o servidor web).  
  **Nota:** Esta biblioteca não está no Gerenciador de Bibliotecas. Você deve baixá-la manualmente:
  1. Acesse [https://github.com/me-no-dev/ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer).
  2. Clique em **Code > Download ZIP**.
  3. Na Arduino IDE, vá em **Sketch > Incluir Biblioteca > Adicionar Biblioteca .ZIP** e selecione o arquivo baixado.
- **AsyncTCP** (necessária para o ESPAsyncWebServer):
  1. Acesse [https://github.com/me-no-dev/AsyncTCP](https://github.com/me-no-dev/AsyncTCP).
  2. Baixe o ZIP e adicione da mesma forma.
- **Preferences** (já incluída no pacote ESP32, não precisa instalar separadamente).
- **ESPmDNS** (já incluída no pacote ESP32).
- **DNSServer** (já incluída no pacote ESP32).

### 4. Baixar e Carregar o Código
1. Baixe o código do projeto:
- No repositório do GitHub, clique em **Code > Download ZIP** ou copie o arquivo `esp32_safecare.ino` fornecido.
- Se baixou o ZIP, extraia a pasta e abra o arquivo `.ino` na Arduino IDE.
2. Conecte o ESP32 ao computador com o cabo USB.
3. Na Arduino IDE:
- Vá em **Ferramentas > Placa** e selecione **ESP32 Dev Module** (ou o modelo específico do seu ESP32).
- Em **Ferramentas > Porta**, selecione a porta onde o ESP32 está conectado (ex.: `COM3` no Windows ou `/dev/ttyUSB0` no Linux/macOS). Se não souber, desconecte o ESP32, verifique as portas, conecte novamente e veja qual nova porta aparece.
4. Clique no botão **Upload** (seta para a direita) na Arduino IDE. Aguarde até que o processo termine (pode levar alguns segundos).
5. Abra o **Monitor Serial** (ícone de lupa no canto superior direito da Arduino IDE) e defina a taxa de bauds para `115200`. Isso mostrará mensagens do ESP32, como o status da conexão Wi-Fi.

### 5. Conectar o ESP32 ao Wi-Fi
Quando o ESP32 inicia, ele tenta se conectar a uma rede Wi-Fi salva anteriormente. Se não houver credenciais salvas ou a conexão falhar, ele criará uma rede Wi-Fi própria (Access Point). Siga os passos:

1. **Conectar à rede SafeCare-AP**:
- No seu computador ou celular, abra as configurações de Wi-Fi.
- Procure pela rede chamada `SafeCare-AP`.
- Conecte-se usando a senha: `safecare123`.
2. **Configurar as credenciais Wi-Fi**:
- Abra um navegador (Chrome, Firefox, etc.) e digite qualquer endereço (ex.: `http://example.com`). Você será redirecionado automaticamente para a página de configuração do ESP32 (ou acesse diretamente `http://safecare.local/wifi`). {ou seu ip se tiver usando um monitor serial}
- Na página, insira o nome da sua rede Wi-Fi (SSID) e a senha da sua rede Wi-Fi doméstica.
- Clique em **Salvar**. O ESP32 reiniciará e tentará se conectar à rede Wi-Fi que você configurou.
3. **Verificar a conexão**:
- No Monitor Serial da Arduino IDE, observe as mensagens. Se a conexão for bem-sucedida, você verá algo como:
  ```
  Wi-Fi conectado!
  [Endereço IP, ex.: 192.168.1.100]
  mDNS iniciado: safecare.local
  ```
- Se a conexão falhar, o ESP32 voltará ao modo Access Point, e você pode tentar novamente.

### 6. Acessar as Páginas no Computador
Depois que o ESP32 estiver conectado à sua rede Wi-Fi, você pode acessar as duas páginas do projeto:

1. **Página principal (SafeCare Dashboard)**:
- Abra um navegador no seu computador (conectado à mesma rede Wi-Fi que o ESP32).
- Digite o endereço: `http://safecare.local/home`.
- A página mostrará os dados do acelerômetro (X, Y, Z) em tempo real, alertas de queda ou tremor, e um gráfico com os valores do eixo Z.
- Se `safecare.local` não funcionar (alguns dispositivos ou redes não suportam mDNS), use o endereço IP do ESP32 mostrado no Monitor Serial (ex.: `http://192.168.1.100/home`).
- Nesta página, há um link **Configurar Wi-Fi** que leva à página de configuração.
2. **Página de configuração Wi-Fi**:
- Acesse: `http://safecare.local/wifi` (ou o IP do ESP32, ex.: `http://192.168.1.100/wifi`).
- Use esta página para alterar o nome e a senha da rede Wi-Fi, se necessário. Insira as novas credenciais e clique em **Salvar**. O ESP32 reiniciará e tentará se conectar com as novas credenciais.
- Há um link **Voltar para Home** para retornar à página principal.
- 

### 7. Usar o Projeto
- **Monitoramento**: A página em `http://safecare.local/home` atualiza os dados do sensor a cada 500ms, mostrando se uma queda ou tremor prolongado foi detectado.
- **Alterar Wi-Fi**: Se precisar mudar a rede Wi-Fi (ex.: em um novo local), acesse `http://safecare.local/wifi`, insira as novas credenciais e salve.
- **Solucionar problemas**:
- Se não conseguir acessar `safecare.local`, use o endereço IP do ESP32 (verifique no Monitor Serial).
- Se o ESP32 não conectar ao Wi-Fi, ele voltará ao modo `SafeCare-AP`. Conecte-se novamente a essa rede e configure as credenciais corretas.
- Verifique se o sensor MPU9250 está corretamente conectado (consulte a documentação do sensor para pinagem).

## Conexão do Hardware
Certifique-se de que o sensor MPU9250 está conectado ao ESP32. As conexões típicas são:

- **VCC** do MPU9250 ao **3.3V** do ESP32.
- **GND** do MPU9250 ao **GND** do ESP32.
- **SCL** do MPU9250 ao pino **22** do ESP32 (padrão I2C).
- **SDA** do MPU9250 ao pino **21** do ESP32 (padrão I2C).  
Consulte o datasheet do seu módulo MPU9250 para confirmar os pinos.

## Dicas
- **Primeira vez**: O ESP32 sempre inicia no modo Access Point (`SafeCare-AP`) se não tiver credenciais salvas, então siga o passo 5 para configurar o Wi-Fi.
- **Rede Wi-Fi**: Certifique-se de que a rede Wi-Fi que você configurar tenha acesso à internet, caso contrário, o ESP32 pode demorar mais para conectar.
- **Navegador**: Use um navegador atualizado para evitar problemas com a interface web.
- **Monitor Serial**: Mantenha o Monitor Serial aberto durante os testes para ver mensagens de erro ou o IP do ESP32.
