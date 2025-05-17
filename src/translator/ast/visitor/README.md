# AST 방문자 패턴 구현

이 디렉토리에는 AST(추상 구문 트리)에 적용할 방문자 패턴 구현이 포함되어 있습니다.

## 방문자 패턴 개요

방문자 패턴은 알고리즘을 데이터 구조에서 분리하는 디자인 패턴입니다. 이를 통해 AST 구조를 변경하지 않고도 새로운 연산을 추가할 수 있습니다.

### 클래스 설명

- `IASTVisitor`: 방문자 인터페이스로, 모든 AST 노드 타입에 대한 Visit 메서드를 선언합니다.
- `ASTVisitor`: 기본 방문자 구현으로, 모든 Visit 메서드가 비어 있습니다. 상속받은 클래스에서 필요한 메서드만 구현할 수 있게 합니다.
- `BytecodeGeneratorVisitor`: AST를 바이트코드로 변환하는 구체적인 방문자 구현입니다.

## 사용 방법

1. `IASTVisitor`를 상속받아 필요한 방문자 클래스를 정의합니다.
2. 각 AST 노드 타입에 대한 `Visit` 메서드를 구현합니다.
3. AST 노드는 `Accept` 메서드를 통해 방문자를 받아들입니다.

```cpp
// 방문자 생성
BytecodeGeneratorVisitor visitor;

// AST 순회
rootNode->Accept(visitor);

// 결과 사용
std::vector<uint8_t> bytecode = visitor.GetBytecode();
```

## 사용 사례

- 바이트코드 생성: `BytecodeGeneratorVisitor`
- 상수 폴딩: `ConstantFoldingVisitor`
- 코드 출력: `PrintVisitor`
- 정적 분석: `TypeCheckVisitor`

## 확장

새로운 방문자를 추가하려면:

1. `ASTVisitor`를 상속받는 새 클래스를 생성합니다.
2. 필요한 `Visit` 메서드를 오버라이드합니다.
3. 필요한 내부 상태와 도우미 메서드를 추가합니다. 