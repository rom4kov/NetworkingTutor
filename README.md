```
      _   __       __                          __    _               
     / | / /___   / /_ _      __ ____   _____ / /__ (_)____   ____ _ 
    /  |/ // _ \ / __/| | /| / // __ \ / ___// //_// // __ \ / __ `/ 
   / /|  //  __// /_  | |/ |/ // /_/ // /   / ,<  / // / / // /_/ /  
  /_/ |_/ \___/ \__/  |__/|__/ \____//_/   /_/|_|/_//_/ /_/ \__, /   
                   ______        __                        /____/    
                  /_  __/__  __ / /_ ____   _____                   
                   / /  / / / // __// __ \ / ___/                    
                  / /  / /_/ // /_ / /_/ // /                        
                 /_/   \__,_/ \__/ \____//_/                        
```

**NetworkingTutor** is a text-based, IDE-like learning environment designed
to teach students the fundamentals of network programming in C. It provides
a guided and interactive platform where users can create an account, enroll
in courses, complete coding assignments, and receive automated feedback.
The goal is to make learning network programming approachable while still
being rigorous, combining educational material with hands-on projects and
a layer of gamification to keep users motivated.

https://github.com/user-attachments/assets/023925d3-6d94-4b06-a270-c99dc5fd1c68

#### Another Video Demo with some explanation (bad audio for now, will make a better one later): [https://youtube.com/2se83kjd7](https://youtu.be/YWNfvbsoCuA)

The first available course, *Build a Simple HTTP Server in C*, introduces
students to a classic project for solidifying the basics of C while learning 
the foundations of internet communication. Later courses (currently planned 
but not yet fully implemented) extend this foundation with modules on HTTP 
parsing, routing, and additional features of network servers.

Each course is divided into sections that present core concepts and culminate
in coding assignments. Students must submit their solutions to automated
tests in order to progress to subsequent sections. Points are awarded for
completed assignments, sections, and courses, and accumulated points unlock
higher user ranks within the built-in gamification system.

---

### **Start View**
<img width="1920" height="1079" alt="screenshot_2025-09-24-040745" src="https://github.com/user-attachments/assets/c6fdb699-3e2f-4ece-a674-116dbd968350" />

After creating an account, the user is greeted with a start screen that provides
a welcome message, an overview of how to use the program, and summaries of
available courses. From here, the user can choose a course to begin. Entering
a course opens the **Course View**, the central workspace that combines learning
materials with coding and testing tools.

---

### **Course View**

The Course View is divided into multiple windows that together provide a
lightweight, IDE-like experience:

#### File Explorer
The file explorer allows users to create, open, rename, and delete files
and directories. It also refreshes to display files created externally (
e.g., via the terminal). Internally, the explorer is implemented with a doubly
linked list of directory entry structures.

#### Editor
The text editor is intentionally simple but functional, supporting cursor
navigation via arrow keys, indentation with the `TAB` key, and shortcuts
like `Home` and `End`. Basic syntax highlighting for C is included, along
with a status line showing metadata and a buffer label displaying the filename
and modification state. The editor’s core data structure is a doubly linked
list of line structures, inspired by how editors like *nano* handle text
internally.

#### Terminal
The built-in terminal is a command execution window rather than a full interactive
shell. It enables compiling, running, and debugging programs directly within
the interface. Instead of using `forkpty()` to provide a full pseudo-terminal
, it relies on the simpler `popen()` function to execute commands and capture
their output. This design is sufficient for the educational purpose of the tool.

#### Instructions Window
This window presents the course materials: explanations of key concepts,
step-by-step implementation guidance, and assignment instructions. Text is
revealed progressively, and the window is scrollable with navigation between
sections. After code is submitted, this window also displays test results
, which are implemented using the lightweight [CUnit](http://cunit.sourceforge.net/) testing framework.

#### Status Bar
A small status display in the bottom right of the course view shows the active
course and the number of completed sections. Its role is minor in the current
version but provides a foundation for future expansion.

<img width="1921" height="1080" alt="screenshot_2025-09-24-040308" src="https://github.com/user-attachments/assets/85dc0803-616c-4243-b37f-e98fdef45aab" />
---

### **Other Views**

- **All Courses View**: Displays upcoming and planned courses (currently
placeholders).
- **Account & Progress View**: Shows account information, completed sections
, total points, rank, and detailed progress across all courses.
- **Keybindings View**: Lists all available keybindings to help users navigate
and operate the program efficiently.

---

### **How to Get Started**

#### Prerequisites
NetworkingTutor is written in C and depends on several external libraries.  
Make sure the following packages are installed on your system:

- **ncurses**
- **SQLite**
- **PCRE2 version 10.45 or newer** ← **important!**
- **CUnit**
- **valgrind**

##### Arch Linux:
```
sudo pacman -S ncurses sqlite pcre2 cunit valgrind
```

##### Linux Mint:
```
sudo apt install libncurses-dev libncursesw5-dev libsqlite3-dev libcunit1-dev libpcre2-dev valgrind
```


> ⚠️ Note: The version of PCRE2 available in some distributions (e.g., Linux Mint) may be older than 10.45.  
> If your package manager does not provide version 10.45, you need to compile it manually.

#### Installing PCRE2 10.45 manually
1. Download the source archive from the [PCRE2 project page](https://www.pcre.org/).  
2. Extract the archive and build the library:
   ```bash
   tar -xvf pcre2-10.45.tar.gz
   cd pcre2-10.45
   ./configure
   make
   sudo make install

#### Verify the installation:

```
pcre2-config --version
```

This should output 10.45.

#### Building NetworkingTutor

Once dependencies are installed, clone the repository and build the program:

```bash
git clone https://github.com/rom4kov/NetworkingTutor.git
cd NetworkingTutor
make
```

This will create the ntutor executable in the project root directory.

#### Running NetworkingTutor

Start the program by running:

```bash
./ntutor
```

💡 Note: NetworkingTutor works best with the **Ghostty** terminal emulator. Other terminal emulators, like **Alacritty**, generally work as well but may occasionally be less smooth or display some characters differently.


---

### **Architecture and Design**

NetworkingTutor is structured around a few central design principles:

1. **Central Input Loop**
   A global input loop captures all keypresses and delegates them to specialized
handler functions depending on the active view and window.

2. **Global Context Struct**  
   A single application context structure is initialized in `main()` and
passed down into relevant functions. This provides a straightforward and
efficient way to maintain application state without relying on global variables.

3. **Directory Organization**  
   The codebase loosely follows the *Model–View–Controller (MVC)* pattern:  
   - **Models (`models/`)**: Define core data structures such as the global app context, text buffers, and user/course data.  
   - **Views (`views/`)**: Render windows and present data to the user, such as `course_view.c` or `editor_view.c`.  
   - **Controllers (`controllers/`)**: Handle user input and connect models with views, such as `editor_controller.c` and `explorer_controller.c`.  
   - **Core (`core/`)**: Provides essential logic like the text editor (`editor.c`), the file explorer (`explorer.c`), and input handlers.  
   - **Data (`data/`)**: Manages persistence, including file I/O (`file_io.c`) and SQLite interaction (`data_access_layer.c`).  
   - **Course Tests (`course_tests/`)**: Automated test cases that verify assignment submissions.

This structure keeps concerns well separated while remaining simple enough
for a C-based, TUI application. While it does not implement MVC in the strictest
sense (especially since views and controllers sometimes overlap in responsibility
), the directory layout and code organization are clearly inspired by MVC
principles.

---

### **Current Scope and Future Work**

At present, NetworkingTutor includes one fully developed course: *Build an
HTTP Server in C*. This serves as a minimum viable product (MVP) and proof
of concept for the platform. Planned extensions include additional courses
, richer gamification features, improved test feedback, and enhancements
to the editor and terminal. The goal is to gradually evolve NetworkingTutor
into a comprehensive learning tool for network programming.

---

### **Platform Support**

NetworkingTutor has been tested on **Arch Linux**, **Ubuntu** and **Linux Mint**.  
The program has not yet been ported or tested on macOS or Windows.

---

### **References**

- Padala, Pradeep and Dickey, Thomas (2024). *NCURSES Programming HOWTO*.<br>
  Available at: [https://invisible-island.net/ncurses/howto/NCURSES-Programming-HOWTO.html](https://invisible-island.net/ncurses/howto/NCURSES-Programming-HOWTO.html)<br>
  My main reference and source for learning ncurses. 
- Hall, Brian “Beej” (2024). *Beej’s Guide to Network Programming*. Available at: [https://beej.us/guide/bgnet/](https://beej.us/guide/bgnet/)  
  This resource was instrumental in writing the material for the course *Build an HTTP Server in C*.
