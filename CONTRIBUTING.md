# Contributing to Genesis Platformer

Thank you for your interest in contributing to this Sega Genesis platformer game!

## Why We Use Pull Requests

Pull requests are an essential part of our development workflow. Here's why we use them:

### 1. **Code Review and Quality**
Pull requests allow other developers to review your changes before they're merged into the main codebase. This helps:
- Catch bugs and potential issues early
- Ensure code follows project conventions
- Share knowledge across the team
- Maintain code quality standards

### 2. **Documentation of Changes**
Each pull request serves as a record of:
- What was changed and why
- The problem being solved
- Discussion and decisions made during development
- Historical context for future reference

### 3. **Safe Integration**
Pull requests provide a safe way to:
- Test changes in isolation before merging
- Run automated tests and checks
- Verify that new code doesn't break existing functionality
- Gradually integrate features without disrupting the main branch

### 4. **Collaboration**
Pull requests facilitate:
- Discussion about implementation approaches
- Feedback from multiple contributors
- Knowledge sharing about the codebase
- Building a stronger community around the project

### 5. **Version Control Best Practices**
Pull requests help us:
- Keep the main branch stable and deployable
- Track all changes systematically
- Revert problematic changes if needed
- Maintain a clean commit history

## How to Contribute

### Setting Up Your Development Environment

1. **Install Required Tools:**
   - [SGDK](https://github.com/Stephane-D/SGDK) - C SDK for Sega Genesis
   - [LDTK](https://ldtk.io/) - 2D level editor (optional)
   - [KEGA FUSION](https://segaretro.org/Kega_Fusion) - Emulator for testing

2. **Fork and Clone:**
   ```bash
   git clone https://github.com/YOUR-USERNAME/Genesis_platformer_1.git
   cd Genesis_platformer_1
   ```

3. **Build the Project:**
   Follow the SGDK documentation to compile the ROM
   
4. **Test Your Changes:**
   Load `out/rom.bin` in your Sega Genesis emulator or on real hardware with an Everdrive

### Making a Pull Request

1. **Create a New Branch:**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make Your Changes:**
   - Write clean, readable code
   - Follow the existing code style
   - Test thoroughly on an emulator or hardware

3. **Commit Your Changes:**
   ```bash
   git add .
   git commit -m "Description of your changes"
   ```

4. **Push to Your Fork:**
   ```bash
   git push origin feature/your-feature-name
   ```

5. **Open a Pull Request:**
   - Go to the original repository on GitHub
   - Click "New Pull Request"
   - Select your branch
   - Describe your changes clearly
   - Reference any related issues

### Pull Request Guidelines

- **Clear Description:** Explain what your PR does and why
- **Small, Focused Changes:** Keep PRs focused on a single feature or fix
- **Test Your Code:** Ensure the ROM builds and runs correctly
- **Be Responsive:** Address feedback and questions promptly
- **Be Patient:** Reviews take time, but they make the project better

## Code of Conduct

- Be respectful and constructive in all interactions
- Help newcomers learn and grow
- Focus on the code, not the person
- Celebrate contributions of all sizes

## Questions?

If you have questions about contributing or pull requests, feel free to:
- Open an issue for discussion
- Ask in your pull request
- Review existing PRs to see examples

Thank you for helping make this Genesis platformer better!
