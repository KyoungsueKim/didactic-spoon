# PMS01 Power Monitor with MQTT Dashboard

이 프로젝트는 PMS01 보드와 PZEM-004T 모듈을 사용하여 전력을 모니터링하고, 데이터를 MQTT를 통해 Node-RED 대시보드로 전송하는 시스템입니다.

## 기능

- 실시간 전력 측정 및 모니터링
- MQTT를 통한 데이터 전송
- Node-RED 대시보드를 통한 데이터 시각화
- 과거 데이터 조회 및 분석

## 하드웨어 요구사항

- PMS01 보드
- PZEM-004T v3.0 모듈
- WiFi 연결이 가능한 네트워크 환경

## 소프트웨어 요구사항

### Arduino IDE 설정

1. Arduino IDE 설치
    - [Arduino 공식 웹사이트](https://www.arduino.cc/en/software)에서 다운로드

2. 필요한 라이브러리 설치
    - PubSubClient
    - ArduinoJson
    - 기존 PMS01 관련 라이브러리들

### Node-RED 설치 및 설정

1. Node-RED 설치
   ```bash
   sudo npm install -g --unsafe-perm node-red
   ```

2. 필요한 Node-RED 패키지 설치
    - node-red-dashboard
    - node-red-contrib-influxdb
    - node-red-contrib-mqtt-broker
   ```bash
   cd ~/.node-red
   npm install node-red-dashboard node-red-contrib-influxdb node-red-contrib-mqtt-broker
   ```

3. InfluxDB 설치 (데이터 저장용)
   ```bash
   sudo apt-get update
   sudo apt-get install influxdb
   sudo systemctl enable influxdb
   sudo systemctl start influxdb
   ```

### 프로젝트 구조

```
PowerMonitor/
├── PowerMonitor.h         # 전력 모니터링 클래스 헤더
├── PowerMonitor.cpp       # 전력 모니터링 클래스 구현
├── MQTTClient.h          # MQTT 클라이언트 클래스 헤더
├── MQTTClient.cpp        # MQTT 클라이언트 클래스 구현
└── PowerMonitorExample.ino # 메인 스케치 파일
```

## 설정 방법

1. Arduino 코드 설정
    - PowerMonitorExample.ino 파일에서 WiFi 및 MQTT 설정 수정
   ```cpp
   const char* WIFI_SSID = "your_ssid";
   const char* WIFI_PASSWORD = "your_password";
   const char* MQTT_SERVER = "192.168.1.100";  // Node-RED 서버 IP
   const int MQTT_PORT = 1883;
   ```

2. Node-RED 흐름 설정
    - power-monitor-flow.json 파일을 Node-RED에 import
    - MQTT 브로커 설정 확인
    - InfluxDB 연결 설정

3. InfluxDB 데이터베이스 생성
   ```sql
   CREATE DATABASE power_monitor
   CREATE RETENTION POLICY "one_year" ON "power_monitor" DURATION 52w REPLICATION 1 DEFAULT
   ```

## 대시보드 기능

1. 실시간 모니터링
    - 현재 전력 사용량
    - 전압, 전류, 역률 게이지
    - 실시간 전력 소비 그래프

2. 히스토리 분석
    - 일별/월별 전력 소비량
    - 시간대별 사용 패턴
    - 최대/최소 전력 사용량

3. 기간별 조회
    - 날짜 선택을 통한 특정 기간 데이터 조회
    - 기간별 전력 사용량 비교
    - 소비 패턴 분석

## 데이터 형식

### MQTT 메시지 형식
```json
{
    "timestamp": "2024-01-01 12:34:56",
    "voltage": 220.5,
    "current": 1.23,
    "power": 270.8,
    "energy": 1.234,
    "frequency": 60.0,
    "powerFactor": 0.98
}
```

### InfluxDB 데이터 구조
- Measurement: power_measurements
- Tags: device_id
- Fields:
    - voltage
    - current
    - power
    - energy
    - frequency
    - powerFactor

## 트러블슈팅

1. MQTT 연결 문제
    - MQTT 브로커 IP 주소 확인
    - 방화벽 설정 확인 (포트 1883)
    - WiFi 연결 상태 확인

2. 데이터 수집 문제
    - Serial 모니터를 통한 디버그 메시지 확인
    - PZEM 모듈 연결 상태 확인
    - I2C 통신 상태 확인

3. Node-RED 대시보드 문제
    - 브라우저 콘솔 로그 확인
    - InfluxDB 연결 상태 확인
    - Node-RED 로그 확인

## 유지보수

1. 정기적인 점검사항
    - InfluxDB 데이터 백업
    - 시스템 로그 확인
    - 센서 정확도 검증

2. 데이터 관리
    - 오래된 데이터 정리
    - 데이터베이스 최적화
    - 백업 및 복구 계획 수립

## License

This project is licensed under the MIT License - see the LICENSE file for details

## 기여

버그 리포트, 기능 제안, Pull Request 등 모든 기여를 환영합니다.