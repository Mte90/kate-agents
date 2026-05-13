# Kate Agent Plugin

An AI assistant integrated into Kate that replicates Zed Editor's agent functionality: LLM chat, tool execution, context from open buffers, and iteration loops.

## ✨ Features

### 🤖 LLM Chat with Streaming
- Real-time chat with token streaming support
- Compatible with any OpenAI-compatible API (OpenAI, Anthropic, Ollama, LM Studio, etc.)
- Flexible configuration: API key, base URL, model, system prompt

### 🔧 Tool Execution
The plugin automatically executes tools based on LLM instructions:

| Tool | Description |
|------|-------------|
| **ReadFile** | Reads files from the filesystem |
| **EditFile** | Modifies files with applyable diffs |
| **Grep** | Searches patterns in the project |
| **Terminal** | Executes commands (sandboxed) |
| **WebSearch** | Searches via DuckDuckGo |

**Security sandboxing:** Terminal commands block `rm`, `curl`, `wget`, `sh`, `bash`, `cp`, `mv`, `python`, `perl`, `ruby` for protection.

### 📚 Automatic Buffer Context
- Automatically detects files open in Kate
- Injects context into LLM prompts (limit: 2000 characters)
- Configurable toggle in settings

### 🔄 Intelligent REPL Loop
- LLM → Tool → Result → LLM (automatic cycle)
- Up to 20 iterations to prevent infinite loops
- Tool output displayed inline in the chat

### 📋 Multiple Chat Tabs
- Support for multiple concurrent chats
- Each tab has its own conversation history
- Close individual tabs with X button
- Create new chats with + button
- Automatic tab naming (Chat 1, Chat 2, etc.)
- Tab titles update based on first message

### 💾 Conversation Persistence
- Automatic thread saving to JSON files
- Conversation history loading on startup
- **Project-based isolation**: Chats are organized by git repository or file
- Path: `~/.config/kate/agents/`
- File naming:
  - Git repos: `{repo-name}_chat_YYYYMMDD_N.json` (e.g., `kate-agents_chat_20260513_1.json`)
  - Single files: `{filename}_chat_YYYYMMDD_N.json` (e.g., `main.cpp_chat_20260513_1.json`)
  - Directories: `{dirname}_chat_YYYYMMDD_N.json` (e.g., `projects_chat_20260513_1.json`)
- Auto-save every 30 seconds
- Auto-save when switching tabs
- Auto-save after each turn completes

### 👤 Agent Profiles
Three modes for different scenarios:

| Profile | Use |
|---------|-----|
| **Write** | Code writing, substantial changes |
| **Ask** | Questions, explanations, debugging |
| **Minimal** | Concise responses, reduced context |

### @-Mentions for Files and Web
- Type `@` for file autocomplete
- Fuzzy search on project paths
- Support for URLs and web searches

### 📊 Diff Preview
- Side-by-side preview before applying changes
- Multi-file support
- Explicit user confirmation required

### 💾 Checkpoint Backups
- Timestamped backups before every modification: `.bak.YYYYMMDD-HHMMSS`
- Maximum 5 backups per file (oldest are deleted)
- Easy recovery of previous file versions

### 👻 Ghost Text (Inlay Hints)
- Inline suggestions while typing
- Accept with Tab key
- Implemented via KTextEditor InlineNoteProvider API

### 📋 Context Menu
- Right-click in editor → "Ask agent about this"
- Sends current selection to the agent

### ⚙️ Configuration Page
Accessible from: `Settings → Configure Kate → Plugins → Kate Agent`

Available settings:
- API Key
- Base URL (for self-hosted providers)
- Model (e.g., `gpt-4`, `claude-3`, `llama3`)
- Custom system prompt
- Buffer context toggle (enabled by default)

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

### API Configuration

1. Open the Agent panel (`Ctrl+Alt+A`)
2. Click the gear icon (⚙️)
3. Enter:
   - **API Key**: Your API key
   - **Base URL**: `https://api.openai.com/v1` (or self-hosted)
   - **Model**: `gpt-4o`, `claude-3-5-sonnet`, `llama3:70b`, etc.
   - **System prompt**: (optional) customize behavior

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

### Create Checkpoints

Before important modifications:
1. Click the "checkpoint" icon in the panel
2. A timestamped backup will be created
3. Recover from `file.bak.YYYYMMDD-HHMMSS`

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
| Test UI/E2E | ⚠️ Partial | 4/7 unit tests passing |

## 🧪 Testing

```bash
# Run all tests
cd build
ctest --output-on-failure

# Specific tests
./tests/test_configmanager
./tests/test_checkpointmanager
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
