# NetworkingTutor
#### Video Demo: youtube.com/2se83kjd7
#### Description:
NetworkingTutor is TUI based program that provides users a way to learn 
network programming in a simple and easy to use, IDE-like software platform.
The user can create an account and start to take courses. These are mostly small 
to medium sized coding adventures focusing on coding up a working program in the 
realm of network programming or an important feature of such a program. The first 
course, for example is called "Build a simple HTTP server in C", a classic project
for students who just learned the basics of C and want to fortify and expand 
their knowledge in a hands-on project. The nexting courses add basic features to this
foundation, like an HTTP parser and an HTTP router.
Each course is divided into several sections that conclude with coding assignments
which the user has to work on and submit to automatic tests to unlock the next
section. For completing sections and courses the user is awarded points and can
rank up in the builtin gamification system the purpose of which is to keep the user
motivated and engaged.

After creating an account the user gets to the start screen where there is a 
short welcome text, and introduction on how to use the program and the short
summaries of the first courses, that can be taken. Upon picking the first
course the user gets to the course view of the program that includes a simple
file explorer, a code editor, a simple terminal that can be toggled on and off
and used by the user to compile, test and debug the code the was written for the 
assignments. It also features a window that shows the course material and 
instructions. The same window also displays the test results for submitted code 
assignments.

##### **Course view**
###### File explorer:
The builtin file explorer allows the user to create, open, rename and delete
files and directories. When creating files in the terminal or otherwise the
file explorer can be updated to show these externally created content.

###### Editor:
The editor is a very simple text editor. The user can navigate using the arrow keys,
the TAB key for indentation etc. There basic syntax highlighting for C code, some
metadata in a status line, and the name of the file and modification state displayed
in a buffer label at the top.

###### Terminal:
The Terminal can be toggled on and off. It's not a real terminal with the ability
to run fully interactive programs but more like a command execution window. Simple
compiling, testing and debugging the code written in the editor is its main purpose.

###### Instructions window:
Here the user is shown course materials like step-by-step explanations of the topic 
at hand and instructions on how to implement the project details and features.
Course material can be uncovered paragraph by paragraph. The window is scrollable and
it's possible to navigate between course sections. This is also the place where
test results are shown for submitted coding assignments.

###### Status bar
A small status window in the bottom right of the course view displays the current
course's name and the number of completed sections in that course. It's functionality
will be expanded in the future.

##### **All courses view**
There's a special view for displaying all courses that are planned to be available
in the future but are still work in progress.

##### **Account & progress view**
Another view shows account details and the current progress of the user. The progress
details include the number of completed courses, course sections and items (paragraphs),
as well as collected points and the current rank of the user based on collected points.
This view also shows which courses have been started and what percentage of
them has been completed so far.

##### **Keybindings view**
The last view lists all keybindings needed to use the program.

##### <ins>**Basic architecture and code design of the project**</ins>
The project is build around several core mechanisms: 
1. a global input loop, that listens for key presses from the user at a central
place in the code and delegates the input to special handler functions based on
which view and window is currently active.

2. A global context struct that is initialized in the top main function and passed
down (sometimes only the relevant parts of the context) to every function and input
handler that needs it. This is an incredibly simple and efficient way to manage
global and local app state.
