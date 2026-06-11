# Kate Agent Plugin

An AI coding assistant for [Kate](https://kate-editor.org/) — chat with any OpenAI-compatible LLM, execute tools on your codebase, and iterate automatically, all from inside the editor.

<details>
<summary>Screenshots</summary>

<img width="1105" height="594" alt="Kate Agent chat panel with streaming response" src="https://github.com/user-attachments/assets/3b95cecf-9c1b-4a8e-b0f4-36b716e3beda" />
<img width="1807" height="916" alt="Diff preview and tool execution results" src="https://github.com/user-attachments/assets/8e7a4c0d-2549-42b1-867e-1db79b49b41a" />

</details>

## ✨ Features

### 🤖 LLM Chat with Real-time Streaming
- Receive AI responses as they stream, with live markdown rendering and syntax highlighting
- Compatible with any OpenAI-compatible API (OpenAI, Anthropic, Ollama, LM Studio, etc.)
- Model selection dropdown to switch between different AI models
- Agent profiles: **Write** (code changes), **Ask** (questions/debugging), **Minimal** (concise)
- Configurable system prompt per profile
- Keyboard shortcuts: `Ctrl+Enter` to send message, `Enter` for new line

### 🔧 Tool Execution (12 Tools)
The AI executes tools automatically based on your request. Tools requiring permission prompt before running:

| Tool | Description | Permission |
|------|-------------|------------|
| **read** | Reads file contents to understand your code | No |
| **grep** | Searches for patterns across project files | No |
| **findpath** | Finds files and directories by glob pattern | No |
| **listdirectory** | Lists directory contents with type info | No |
| **createdirectory** | Creates new directories (recursive) | Yes |
| **edit_file** | Edits files with diff preview and user confirmation | Yes |
| **apply_diff** | Applies a unified diff patch to a file | Yes |
| **terminal** | Executes shell commands (sandboxed — see security below) | Yes |
| **web_search** | Searches the web via DuckDuckGo, no API key needed | No |
| **url_fetch** | Fetches a webpage and extracts its text content | No |
| **diagnostics** | Gets compiler errors and warnings from LSP | No |

**Terminal security** — blocks destructive or side-effect commands: `rm`, `wget`, `curl`, `sh`, `bash`, `zsh`, `fish`, `cp`, `mv`, `chmod`, `chown`, `mkdir`, `ln`, `touch`, `python`, `perl`, `ruby`, `node`, `php`, `sudo`, `su`, `dd`, `mkfs`, `apt`, `yum`, `pacman`, `systemctl`, `shutdown`, `reboot` and more.

### 📚 Automatic Buffer Context
- Open documents are automatically included in the AI's context
- The agent includes file names, cursors, and selected text
- Helps the AI understand your project without manual file mentions

### 🔄 Intelligent Agent Loop
- **AI → Tool → Result → AI** automatic cycle
- Maximum 20 iterations to prevent runaway loops
- Each tool result appears inline in the chat
- The AI decides which tools to call and in what order

### 📋 Multiple Chat Sessions
- Create unlimited chat tabs with the `+` button
- Each tab maintains its own conversation history
- Rename tabs with the dropdown (`⋯` → Rename)
- Close tabs with the `×` button
- All conversations persist across Kate restarts, organized by git repository

### 📎 File Mentions
- Type `@` in the input bar to open a fuzzy file picker
- Navigate with arrow keys; select with `Enter` or double-click
- Fetched files are automatically included in the AI's context

### 🔍 Right-click Context Menu
- Right-click in Kate's editor → **"Ask Agent About This"**
- Sends the current selection or line to the agent for analysis

### 💾 Conversation Persistence
- Threads save as JSON per project (`{project}_threads.json`)
- Survives Kate restarts; conversations are detected from git repository root
- `Ctrl+U` (or the dropdown → Delete Thread) to clear a conversation

## 📦 Installation

> **Note:** The plugin is built with the `kate-agents` directory name.

### Prerequisites

| Package | Minimum Version |
|---------|-----------------|
| Kate | ≥ 24.08 (KF6) |
| Qt | ≥ 6.5.0 |
| KDE Frameworks | ≥ 6.0.0 |
| CMake | ≥ 3.25 |
| GCC / Clang | C++17 support |

Required KF6 components: `CoreAddons`, `I18n`, `ConfigWidgets`, `TextEditor`, `XmlGui`.

Optional: `KF6SyntaxHighlighting` (enables markdown code highlighting in the chat — falls back gracefully if missing).

### Build and install (quick)

```bash
# Clone / cd into the repo
cd kate-agents

# Build + install (handles chown and cmake automatically)
./build.sh
sudo ./scripts/install.sh
```

The build script handles `build/` ownership; the install script uses `cmake --install` so everything lands in the correct system path.

### Build and install (manual)

```bash
# Fix build directory ownership (one-time, or after root builds)
sudo chown -R $USER:$USER build/

# CMake
cmake -B build -S .
cmake --build build -j$(nproc)

# Install (places plugins/kateagentplugin.so into the system plugin directory)
sudo cmake --install build
```

The plugin is built as `kateagentplugin.so` (no `lib` prefix) via `kcoreaddons_add_plugin`.

### Enable in Kate

1. Open Kate
2. Go to **Settings → Configure Kate → Plugins**
3. Enable **Kate Agent**
4. Restart Kate
5. The panel opens with `Ctrl+Alt+A` (or from the Kate sidebar)

### Configuration

After enabling, configure your LLM provider in Kate's settings sidebar under **Kate Agent → Config**:
- Base URL, API key, and provider name
- Available models load automatically from the provider
- Custom system prompt for each agent profile

### Update

```bash
git pull
./build.sh
sudo ./scripts/install.sh
```

### Uninstall

```bash
sudo ./scripts/uninstall.sh
```

### Troubleshooting

| Problem | Fix |
|---------|-----|
| `build/` owned by root | Run `sudo chown -R $USER:$USER build/` before building |
| Kate doesn't find the plugin | Verify `.so` is in `/usr/lib/x86_64-linux-gnu/qt6/plugins/kf6/ktexteditor/` |
| Config page not showing | Clear Kate cache: `rm -rf ~/.cache/Kate* ~/.local/share/Kate* ~/.config/kate/*` then restart |
| `projectId` shows "default" | Open Kate from *inside* a git repo directory, not from a parent folder |

## 🚀 Usage

### Chat with the Agent

1. Type your request in the input field
2. Use `@` to include specific files from the project
3. Select the profile (Write/Ask/Minimal)
4. Send with `Enter` (or `Ctrl+Enter` for new line)

The agent responds inline and can call tools automatically — reading files, running searches, modifying code with your approval, fetching web content, and executing safe terminal commands.

### Accept Ghost Text Suggestions

When the agent proposes code changes:
- **Tab** — accept the full suggestion
- **Esc** — reject
- **Shift+Tab** — accept line by line

### Thread Management

- **New chat** — click the `+` button (or `Ctrl+U`)
- **Rename** — click `⋯` → Rename, or right-click the tab
- **Delete thread** — click `⋯` → Delete Thread (or `Ctrl+U`)
- **Switch models** — use the dropdown at the top of the panel
- **Stop** — click the stop button to abort an ongoing turn

## ⚠️ Known Limitations

| Feature | Status | Notes |
|---------|--------|-------|
| Multi-file batch edits | ⚠️ Partial | Diff preview works; `edit_file` edits one file at a time |
| Codebase indexing | ❌ No | Only open files and explicit `@` mentions are indexed |

## 🧪 Testing

```bash
# Run all tests from the build directory
cd build
ctest --output-on-failure

# Run with verbose output
ctest -V

# Run a specific test suite
./tests/test_configmanager           # Config manager tests
./tests/test_threadjsonstorage       # Thread persistence tests
./tests/test_agentloop               # Agent loop logic tests
```

The test suite covers core modules (config, storage, agent loop, LLM providers, tools, UI flow) with 90+ test files.

## 🔧 Development

### Quick build

```bash
./build.sh
```

### Build with debug

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
```

### Disable tests

```bash
cmake -S . -B build -DBUILD_TESTS=OFF
```

### Debug logging

```bash
export KATEAGENT_DEBUG=1
kate
```

### Install to `~/.local` (no sudo)

```bash
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build build
cmake --install build

# Copy to local plugins directory
mkdir -p ~/.local/lib/qt6/plugins/kf6/ktexteditor/
cp build/plugins/kateagentplugin.so ~/.local/lib/qt6/plugins/kf6/ktexteditor/
cp src/kateagentplugin.json ~/.local/lib/qt6/plugins/kf6/ktexteditor/
```

### Cache clearing (after updates)

After rebuilding, clear Kate's cache before restarting:

```bash
rm -rf ~/.cache/Kate* ~/.local/share/Kate* ~/.config/kate/*
```
