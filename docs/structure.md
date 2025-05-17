```plantuml
@startuml DarkMatterVM 코드 변환 로직

skinparam {
  DefaultFontName "Noto Sans CJK KR"
  BackgroundColor white
  ArrowColor #2c3e50
  BorderColor #2c3e50
  NodeBackgroundColor #ecf0f1
  NodeBorderColor #7f8c8d
}

rectangle "C++ 코드" as cpp #d5e8d4
rectangle "VM 어셈블리 코드" as vmasm #d5e8d4 

rectangle "Translator 모듈" as translator #dae8fc {
  rectangle "AST 모듈" as ast #ffe6cc {
    rectangle "ASTNode" as astnode
    rectangle "AST 파서" as astparser
  }
  
  rectangle "CodeGen 모듈" as codegen #ffe6cc {
    rectangle "BytecodeBuilder" as bytecodeBuilder
  }
  
  rectangle "Optimizer 모듈" as optimizer #ffe6cc {
    rectangle "ConstantFolding" as constFolding
  }
}

rectangle "Assembler 모듈" as assembler #dae8fc {
  rectangle "Parser" as parser
  rectangle "SymbolTable" as symbolTable
  rectangle "CodeEmitter" as codeEmitter
}

rectangle "VM 바이트코드" as bytecode #f8cecc

cpp -right-> astparser : "파싱"
astparser -right-> astnode : "생성"
astnode -right-> optimizer : "최적화"
optimizer -right-> bytecodeBuilder : "코드 생성"
bytecodeBuilder -down-> bytecode : "출력"

vmasm -down-> parser : "파싱"
parser -right-> symbolTable : "심볼 관리"
symbolTable -right-> codeEmitter : "코드 생성"
codeEmitter -down-> bytecode : "출력"

note bottom of translator
  C++ 코드 변환 경로 (미구현):
  C++ 소스코드 → AST → 최적화 → 바이트코드
end note

note bottom of assembler
  어셈블리 변환 경로 (일부 구현):
  VM 어셈블리 → 파싱 → 바이트코드
  (개발 및 테스트용)
end note

rectangle "Engine 모듈 (Interpreter)" as engine #d5e8d4 {
  rectangle "Decoder" as decoder
  rectangle "Executor" as executor
}

bytecode -down-> decoder : "로드"
decoder -right-> executor : "실행"

@enduml