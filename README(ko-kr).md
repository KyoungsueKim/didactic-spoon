# PMS01 Power Monitor 데모 가이드
<img width="916" alt="image" src="https://github.com/user-attachments/assets/778c7cbd-cac6-4dd8-a6ea-f943715503e4" />

이 문서는 Dignsys사의 PMS01 전력 측정 보드의 간단한 테스트 및 데모 환경을 빠르게 구성하고 동작시킬 수 있도록 안내합니다. 기존에는 아두이노 IDE, Node-RED, MQTT 브로커(mosquitto), InfluxDB를 각각 수동으로 설치/설정해야 했지만, 이번 개편을 통해 모든 백엔드 환경을 **Docker Compose**로 간편하게 구성할 수 있습니다. 또한, PMS01 보드에 업로드되는 펌웨어는 **PlatformIO**를 이용하여 빌드 및 업로드하도록 변경되었습니다.

아래 가이드에 따라 진행하면, PMS01 보드가 로컬 MQTT 서버를 통해 전력 데이터를 보내고, Node-RED 대시보드를 통해 실시간 전력 측정값을 시각화할 수 있습니다.

---

## 주요 기능 개요
<img width="471" alt="image" src="https://github.com/user-attachments/assets/fe8a859e-0f4b-4be7-9a66-9b16900112ff" />

- **PMS01 보드**: PZEM-004T 모듈을 통한 전력(전압, 전류, 전력, 에너지, 주파수, 역률) 측정
- **MQTT 통신**: 로컬 모스키토(Mosquitto) MQTT 브로커에 측정 데이터를 주기적으로 전송
- **Node-RED 대시보드**: InfluxDB를 백엔드로 하여 실시간 및 히스토리 데이터를 시각화
- **Docker Compose 기반 환경**: Node-RED, InfluxDB, Mosquitto를 단일 명령어로 손쉽게 구동

---

## 사전 준비사항

1. **PMS01 보드 및 PZEM-004T 모듈** 준비
2. **PlatformIO** 설치
   - [PlatformIO 공식 사이트](https://platformio.org/) 참고
   - VSCode 확장 프로그램 또는 CLI로 설치 가능
3. **Docker & Docker Compose** 설치
   - [Docker 공식 사이트](https://docs.docker.com/get-docker/) 참고
   - [Docker Compose 설치 안내](https://docs.docker.com/compose/install/) 참고
4. **Wi-Fi 환경** 준비
   - PMS01 보드와 테스트용 PC(혹은 노트북)가 동일한 네트워크(Wi-Fi)에 연결되어야 함
5. PMS01 보드에 12V 전원 공급 및 PZEM-004T 모듈에 220V 공급. **주의: PMS01 보드와는 별개로 PZEM-004T 모듈에 220V 전압이 공급되어야만 정상적으로 동작합니다.**

---

## 프로젝트 구조

```
.
├── README.md                # 프로젝트 사용 방법 및 개요 문서
├── docker-compose.yml       # Node-RED, InfluxDB, Mosquitto 서비스 구성을 위한 Docker Compose 파일
├── include
│   └── README               # include 디렉토리 관련 안내
├── init-influxdb
│   └── init-influxdb.sh     # InfluxDB 초기화 스크립트
├── lib
│   ├── PZEM004Tv30-pms      # PZEM-004T v3.0 센서 라이브러리 (PMS01 보드용)
│   ├── README               # lib 디렉토리 관련 안내
│   └── SC16IS752Serial      # 시리얼 확장 관련 라이브러리
├── mosquitto
│   └── config               # Mosquitto MQTT 브로커 설정 파일
├── platformio.ini           # PMS01 펌웨어 빌드/업로드 설정 파일 (PlatformIO)
├── src
│   ├── MQTTClient.cpp       # MQTT 클라이언트 로직 구현
│   ├── MQTTClient.h
│   ├── PowerMonitor.cpp     # 전력 측정 로직 구현
│   ├── PowerMonitor.h
│   └── main.cpp             # PMS01 펌웨어 메인 진입점
├── test
│   └── README               # 테스트 관련 문서
└── volumes
    ├── influxdb-data        # InfluxDB 데이터 볼륨
    ├── mosquitto-data       # Mosquitto 데이터 볼륨
    ├── mosquitto-log        # Mosquitto 로그 볼륨
    └── nodered-data         # Node-RED 데이터 볼륨

```

---

## PMS01 펌웨어 설정 (PlatformIO)

1. `platformio.ini` 파일에서 PMS01 보드에 맞는 포트 설정(시리얼 포트)과 Wi-Fi, MQTT 설정을 수정합니다.

   ```cpp
   // main.cpp 내 설정 예시
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   const char* MQTT_SERVER = "192.168.0.10"; // docker-compose를 띄운 PC IP
   const int MQTT_PORT = 1883;
   ```

   PMS01 보드와 PC가 같은 네트워크 상에 있어야 하며, `MQTT_SERVER`에 `mosquitto` 컨테이너가 동작하는 호스트(PC)의 IP 주소를 입력하세요.

2. PMS01 보드를 USB로 PC에 연결한 후, PlatformIO를 이용해 펌웨어를 빌드 및 업로드합니다.
   ```bash
   pio run -t upload
   ```

   업로드가 완료되면 PMS01 보드가 Wi-Fi에 연결하고, MQTT 서버로 데이터 전송을 시작합니다.

---

## Docker Compose로 백엔드 서비스 실행

이 프로젝트의 `docker-compose.yml` 파일은 프로젝트 최상위 디렉토리에 위치합니다. 다음 명령을 통해 Node-RED, InfluxDB, Mosquitto 등 백엔드 서비스를 손쉽게 실행할 수 있습니다.

1. Docker Compose 실행
   ```bash
   docker-compose up -d
   ```

   위 명령 실행 후 다음 서비스들이 컨테이너 형태로 구동됩니다:
   - **Node-RED**: [http://localhost:1880](http://localhost:1880)
   - **InfluxDB**: [http://localhost:8086](http://localhost:8086)
   - **Mosquitto (MQTT Broker)**: mqtt://localhost:1883
   - **InfluxDB Admin UI**: [http://localhost:8083](http://localhost:8083) (선택적 확인용)

2. 인프라 상태 확인
   ```bash
   docker ps
   ```
   위 명령으로 컨테이너들이 정상적으로 동작하는지 확인할 수 있습니다.  
   에러나 이슈가 발생한 경우 `docker logs <컨테이너명>`을 통해 로그를 확인하고 문제점을 파악할 수 있습니다.

---

## Node-RED 대시보드 설정

1. Node-RED UI 접속
   - 브라우저에서 `http://localhost:1880` 접속
2. 준비된 Node-RED 플로우 JSON을 가져오기
   - Node-RED 편집기 우측 상단 메뉴에서 Import 선택
   - 제공된 JSON 코드를 붙여넣고 Import
3. Import 후 플로우 Deploy
   - Deploy 버튼을 클릭하여 플로우 활성화
4. Dashboard 확인
   - 브라우저에서 `http://localhost:1880/ui`로 접속하면 전력 측정 대시보드 확인 가능
   - 실시간 파워, 에너지 그래프, 기간별 데이터 조회 등의 기능 확인

---

## 데이터 흐름

1. **PMS01 보드** -> **Mosquitto(MQTT Broker)**  
   PMS01 보드는 설정된 Wi-Fi에 연결 후 `pms01/power` 토픽으로 전력 데이터를 MQTT 전송

2. **Mosquitto** -> **Node-RED**  
   Node-RED는 `pms01/power` 토픽을 Subscribe하여 데이터를 실시간 수신

3. **Node-RED** -> **InfluxDB**  
   Node-RED Flow 내 Function 노드에서 데이터 파싱 후 InfluxDB에 저장

4. **Node-RED Dashboard**  
   InfluxDB에 저장된 전력 데이터를 조회하여 대시보드에서 실시간 그래프 및 통계 확인

---

## 트러블슈팅 가이드

1. **MQTT 연결 실패**
   - PMS01 보드 IP 연결 상태 확인 (보드와 호스트 PC가 같은 LAN에 있는지)
   - Mosquitto 컨테이너가 정상 작동 중인지 `docker logs mosquitto`로 확인
   - 방화벽 설정 확인

2. **데이터 미수신**
   - Node-RED 편집기에서 디버그 노드 추가 후 MQTT-In 노드 출력을 확인
   - PMS01 보드 시리얼 모니터(PlatformIO Monitor)로 로그 출력 확인

3. **InfluxDB 쿼리 문제**
   - InfluxDB Admin UI(http://localhost:8083) 접속해 DB 및 Measurement 확인
   - Node-RED의 InfluxDB 노드 설정 확인

---

## 유지보수 및 확장

- PMS01 보드 측 펌웨어 수정 시, PlatformIO를 통해 재빌드/업로드
- InfluxDB 데이터 백업/관리
- Node-RED 플로우 수정으로 기능 확장 (알람, 이메일 통지 등)

---

## 라이선스 및 기여

- 본 프로젝트는 MIT 라이선스를 따릅니다.
- 버그 제보, 기능 개선 제안, Pull Request 모두 환영합니다.

---

이제 위 단계들을 순서대로 따라하면, PMS01 보드에서 측정한 전력 데이터가 로컬 Docker Compose 환경에서 구동되는 Mosquitto, InfluxDB, Node-RED 대시보드를 통해 쉽게 시각화 및 분석 가능한 데모 환경을 구축할 수 있습니다.
