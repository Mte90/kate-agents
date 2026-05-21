# Kate Agent Plugin

An AI assistant integrated into Kate that replicates Zed Editor's agent functionality: LLM chat, tool execution, context from open buffers, and iteration loops.

<img width="1105" height="594" alt="Image" src="https://github.com/user-attachments/assets/3b95cecf-9c1b-4a8e-b0f4-36b716e3beda" />
<img width="1912" height="770" alt="Image" src="https://github.com/user-attachments/assets/8ed6a944-bb19-468d-b094-912d7e6e9e7f" />

## ✨ Features

### 🤖 LLM Chat with Real-time Streaming
- Type messages and receive AI responses as they stream in real-time
- Compatible with any OpenAI-compatible API (OpenAI, Anthropic, Ollama, LM Studio, etc.)
- Model selection dropdown to switch between different AI models
- Agent profiles: **Write** (code changes), **Ask** (questions/debugging), **Minimal** (concise)
- Streaming text with proper word wrapping and formatting

### 🔧 Tool Execution
The AI can automatically execute tools based on your request:

| Tool | Description |
|------|-------------|
| **read_file** | Reads file contents to understand your code |
| **edit_file** | Suggests code changes with diff preview |
| **grep** | Searches for patterns across the project |
| **find_files** | Lists files matching a pattern |
| **terminal** | Executes shell commands (sandboxed for safety) |

**Security:** Terminal commands block dangerous operations (`rm`, `curl`, `wget`, `sh`, `bash`, `cp`, `mv`, `python`, `perl`, `ruby`).

### 📚 Automatic Buffer Context
- Files you're currently editing are automatically included in the AI's context
- Helps the AI understand your project without manual file mentions

### 🔄 Intelligent Agent Loop
- AI → Tool → Result → AI (automatic cycle)
- Maximum 20 iterations to prevent infinite loops
- Tool outputs displayed inline in the chat
- AI decides which tools to use based on your request

### 📋 Multiple Chat Sessions
- Create unlimited chat tabs with the `+` button
- Each tab maintains its own conversation history
- Close tabs with the `×` button
- Named automatically (Chat 1, Chat 2, etc.)
- All conversations persist across Kate restarts

### @-Mentions for Quick File Reference
- Type `@` to trigger file autocomplete
- Fuzzy search across your project files
- Files are automatically read and included in the AI's context
- Example: `@src/main.cpp fix the null pointer issue`

### 💾 Conversation Persistence
- All chat histories save automatically
- Survives Kate restarts
- Organized by project (git repository)

## 📦 Installation

### Prerequisites
- Kate ≥ 23.08 (KTextEditor KF6)
- Qt 6.4+
- CMake 3.16+
- GCC/Clang with C++17 support

### Build from source

```bash
cd kate-agent-plugin
cmake -S . -B build
cmake --build build -j$(nproc)
sudo cmake --install build
```

### Manual installation

```bash
# Copy the library
sudo cp build/libkateagentplugin.so \
  /usr/lib/x86_64-linux-gnu/qt6/plugins/kf6/ktexteditor/

# Copy the metadata file
sudo cp src/kateagentplugin.json \
  /usr/lib/x86_64-linux-gnu/qt6/plugins/kf6/ktexteditor/
```

### Enable in Kate

1. Open Kate
2. Go to `Settings → Configure Kate → Plugins`
3. Enable **Kate Agent Plugin**
4. Restart Kate
5. Press `Ctrl+Alt+A` to open the panel

## 🚀 Usage

### Chat with the Agent

1. Type your request in the input field
2. Use `@` to mention specific files
3. Select the profile (Write/Ask/Minimal)
4. Press Enter

The agent will respond and can:
- Read files
- Execute searches
- Modify code (with confirmation)
- Execute safe commands

### Accept Ghost Text Suggestions

When the agent proposes changes:
- **Tab**: Accept the suggestion
- **Esc**: Reject
- **Shift+Tab**: Accept line by line

## ⚠️ Known Limitations

### LSP Integration (NOT available)

**Problem:** The plugin cannot access Kate's integrated LSP Client.

**What's missing:**
- Jump to definition (CTRL+click)
- Cross-file symbol search
- Hover information
- Intelligent completion based on LSP

**Current fallback:**
- Regex search on open files
- Works only for symbols in documents currently open in Kate
- Does not index the entire project

**Temporary solution:**
1. Enable Kate's "LSP Client" plugin (`Settings → Configure Kate → Plugins`)
2. Install appropriate language servers (clangd, rust-analyzer, pylsp)
3. Kate will handle them internally (but my plugin cannot use them directly)

**Future plan:**
- Wait for KTextEditor to expose a public API for the LSP client
- Or implement a standalone LSP client (complex, not priority)

### Other Limitations

| Feature | Status | Notes |
|-------------|-------|------|
| Multi-file edit (batch) | ⚠️ Partial | Diff preview works, batch editing no |
| Codebase indexing | ❌ No | Only open files, not entire project |
| LSP hover | ❌ No | Requires exposed LSP API |
| Go to definition | ❌ No | Requires real LSP |
| Test UI/E2E | ⚠️ Partial | 3/3 unit tests passing (93%) |

## 🧪 Testing

```bash
# Run all tests
cd build
ctest --output-on-failure

# Specific tests
./tests/test_configmanager
./tests/test_editorcontext
./tests/test_threadjsonstorage
```

## 🔧 Development

### Build with debug

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
```

### Local installation (without sudo)

```bash
# Install to ~/.local
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build build
cmake --install build

# Manually copy to ~/.local/lib/qt6/plugins/ktexteditor/
```

### Debug logging

Set environment variable:

```bash
export KATEAGENT_DEBUG=1
kate
```
