# Trabalho 1 - FSE - Controle de Sistema de Wifi e GPIO
aluno: Lucas Penido Antunes  
matrícula: 16/0013143

Repositório com a solução do trabalho 3.

## Projeto

Este trabalho tem por objetivo a familiarização com o framework de desenvolvimento ESP-IDF para a programação do ESP32-Dev-Kit. Mais especificamente o controle do sistema de Wifi juntamente com o acionamento da GPIO.

Para uma maior descrição do projeto visitar este [GitLab](https://gitlab.com/fse_fga/projetos/projeto-3).

### Configurando o programa

Para configurar o Wifi execute o comando:  

`idf.py menuconfig` 

Após o menu abrir selecione a opção "Configuração do Wifi" e configure o SSID e a senha. Depois na opção "Configuração das Chaves das APIs" configure a chave da API do [IpStack](https://ipstack.com/) e a chave da API da [Open Weather Map](https://openweathermap.org/api).  

Após todas as configurações serem feitas rode o seguinte comando para dar o build no projeto.

`idf.py build`

Após feita a build do projeto, rode o seguinte comando para enviar o progama para a ESP32 e monitorá-lo. Trocando (PORT) para a porta que a ESP32 está conectada:

`idf.py -p (PORT) flash monitor`
