Unreal Engine 5로 개발된 멀티플레이어 게임 프로젝트입니다. 플레이어 간 실시간 채팅, 아이템 거래, 인벤토리 관리 시스템을 제공합니다.

## 🎮 주요 기능

### 🗨️ 채팅 시스템
- 실시간 멀티플레이어 채팅
- MessageWidget을 통한 사용자 친화적 UI
- 송신자 이름과 메시지 내용 표시

### 🔄 거래(교환) 시스템
- 플레이어 간 아이템 교환 기능
- 거래 요청/수락/거절 메커니즘
- 실시간 거래 UI
- 안전한 서버-클라이언트 통신을 통한 거래 보장

### 📦 인벤토리 시스템
- 아이템 관리 및 저장
- 드래그 앤 드롭 아이템 이동
- 아이템 추가/제거/교환 기능
- 네트워크 리플리케이션을 통한 동기화

### 🌍 아이템 드롭 시스템
- 월드에 아이템 드롭 가능
- 충돌 감지를 통한 자동 픽업
- 3D 메시를 통한 시각적 표현

## 🏗️ 프로젝트 구조

```
Source/Multiplay/
├── Modes/
│   ├── MultiplayGameMode.h/.cpp          # 게임 모드 설정
├── UI/
│   ├── MessageWidget.h/.cpp              # 채팅 메시지 UI
│   └── TradeRequestWidget.h/.cpp         # 거래 요청 UI
├── Trading/
│   ├── TradeComponent.h/.cpp             # 거래 시스템 컴포넌트
│   └── TradeState.h/.cpp                 # 거래 상태 관리
├── Inventory/
│   ├── InventoryComponent.h/.cpp         # 인벤토리 관리 컴포넌트
│   ├── InventoryTypes.h/.cpp             # 아이템 데이터 구조
│   ├── ItemDragDropOperation.h/.cpp      # 드래그 앤 드롭 기능
│   └── DropItem.h/.cpp                   # 드롭된 아이템 액터
└── Characters/
    └── MultiplayCharacter                 # 플레이어 캐릭터 (참조됨)
```

## 🔧 기술 스택

- **엔진**: Unreal Engine 5.3
- **언어**: C++
- **네트워킹**: Unreal Engine 네이티브 리플리케이션
- **UI**: UMG (Unreal Motion Graphics)
- **빌드 시스템**: Unreal Build Tool

## 🚀 시작하기

### 필요 조건
- Unreal Engine 5.3 이상
- Visual Studio 2022 (Windows)
- Git

### 설치 및 실행

1. **저장소 클론**
   ```bash
   git clone [repository-url]
   cd Multiplay
   ```

2. **프로젝트 파일 생성**
   ```bash
   # .uproject 파일을 우클릭하여 "Generate Visual Studio project files" 선택
   ```

3. **컴파일 및 실행**
   - Visual Studio에서 프로젝트 열기
   - 솔루션 빌드 (Ctrl+Shift+B)
   - Unreal Editor에서 프로젝트 실행

### 멀티플레이어 테스트

1. 에디터에서 **Play** 버튼 옆 드롭다운 메뉴 클릭
2. **Number of Players**를 2 이상으로 설정
3. **Play** 클릭하여 여러 플레이어로 테스트

## 🎯 핵심 컴포넌트

### TradeComponent
플레이어 간 거래를 담당하는 핵심 컴포넌트입니다.

**주요 기능:**
- 거래 요청 송수신
- 거래 세션 관리
- 아이템 교환 처리
- 네트워크 리플리케이션

### InventoryComponent
플레이어의 인벤토리를 관리합니다.

**주요 기능:**
- 아이템 추가/제거
- 슬롯 간 아이템 이동
- 서버-클라이언트 동기화
- 인벤토리 변경 이벤트

### MessageWidget
채팅 메시지를 표시하는 UI 위젯입니다.

**주요 기능:**
- 송신자 이름 표시
- 멀티라인 메시지 지원
- 동적 위젯 초기화

## 🌐 네트워킹 아키텍처

이 프로젝트는 **서버-클라이언트 아키텍처**를 사용합니다:

- **Server RPCs**: 클라이언트에서 서버로의 요청 (거래 신청, 아이템 이동 등)
- **Client RPCs**: 서버에서 클라이언트로의 응답 (UI 업데이트, 상태 변경 등)
- **Replication**: 게임 상태의 자동 동기화


---

**개발 환경**: Unreal Engine 5.3 | **개발 언어**: C++ | **플랫폼**: Windows, Mac, Linux