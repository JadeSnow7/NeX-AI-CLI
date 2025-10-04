# 函数式 Shell

**预留特性**（当前版本尚未完整实现，计划中）

## 设计理念

将文件系统与系统对象映射为不可变值，以函数式风格进行组合与转换。

## 值类型

- `FileRef`：文件引用（路径 + 元数据）
- `String`、`Int`、`Float`、`Bool`
- `List<T>`、`Map<K,V>`
- `Result<T,E>`：带错误的结果
- `SysSeq<Event>`：系统事件序列

## 管道语法

```fnshell
files("*.cpp") | map(text.read) | flat_map(split_lines) | filter(starts_with("// TODO"))
```

## 内置函数

### 文件
- `files(pattern)` → `List<FileRef>`
- `text.read(FileRef)` → `Result<String,Error>`
- `text.write(FileRef, String)` → `Result<Unit,Error>`

### 文本
- `split_lines(String)` → `List<String>`
- `starts_with(prefix)` → `String -> Bool`
- `contains(sub)` → `String -> Bool`

### 系统
- `sys.events()` → `SysSeq<Event>`
- `sys.metrics()` → `SysSeq<Metric>`
- `sys.exec(cmd, args)` → `Result<{stdout,rc},Error>`

### 高阶
- `map(f)` → `List<A> -> List<B>`
- `filter(pred)` → `List<A> -> List<A>`
- `flat_map(f)` → `List<A> -> List<B>`
- `fold(init, f)` → `List<A> -> B`

## 错误处理

```fnshell
text.read(file("tmp.txt"))
  | unwrap_or("default")
  | split_lines
  | map(trim)
```

## 系统查询

```fnshell
sys.events()
  | filter(tag="router")
  | filter(level="error")
  | last(100)
```

## 实现状态

当前版本提供了 schema 定义（`assets/schema/fnshell.ast.schema.json`），实际运行时与解析器计划在后续迭代实现。



