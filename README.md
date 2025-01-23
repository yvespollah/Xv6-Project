# **XV6 Unix System Extension Project Master 1**

## **Introduction**
In this project, we extended the XV6 operating system by implementing user and group management, file permissions, and related system calls to align it with modern Unix system standards. These changes enhance the educational value of XV6 by introducing key operating system concepts such as multi-user functionality, file access control, and process management.

---

## **Objective**
The primary goal was to integrate standard Unix system features, specifically:
1. User and group management.
2. File permission handling.
3. Implementation of new system calls and modification of existing ones.
4. Development of user programs for administration tasks.
5. Extension of XV6’s kernel and user space to support these functionalities.

---

## **What We Did**

### **1. User and Group Management**
- **User Space Integration**: Implemented `/etc/passwd` and `/etc/group` for storing user and group data.  
- **File Formats**:  
  - `/passwd`:  
    ```
    username:password:UID:GID:realname:homedir
    ```
  - `/group`:  
    ```
    groupname:GID:user1,user2
    ```
- **Root User**: Added a superuser (`UID = 0`) with administrative privileges.  
  - Default credentials: `root:password`.

### **2. File Permissions**
- Incorporated Unix-style permissions (`rwx` for owner, group, and others).
- Used octal and symbolic representations (e.g., `0755` or `-rwxr-xr-x`).
- Updated the inode structure to include a permissions field:
  - **Read (r = 4)**: View file or list directory contents.  
  - **Write (w = 2)**: Modify file or directory contents.  
  - **Execute (x = 1)**: Execute file or access directory.

### **3. System Calls**
We added or modified system calls to support the new functionalities:

- **`getuid`**: Retrieve the calling process’s UID.  
- **`setuid`**: Change the calling process’s UID (root-only).  
- **`chmod`**: Change file permissions.  
- **`chown`**: Change file ownership.  
- **Modified `stat`**: Include file ownership and permission details.

#### **Additional System Calls**:
- **`ps`**: 
  - Lists all the processes running on the system along with their PID (Process ID), state, and associated user.
  - This allows the user to monitor and manage processes.
  
- **`nice`**: 
  - Changes the priority of a process.
  - The user can increase or decrease the priority (nice value) of a process, which affects its scheduling.

- **`freemem`**: 
  - Returns the amount of free memory in the system.
  - Useful for memory management and ensuring resources are properly allocated.

- **`lseek`**: 
  - Changes the current position of the file pointer within a file.
  - This system call is crucial for efficient file handling, allowing the user to read from or write to specific file positions.

- **`clear`**: 
  - Clears the terminal screen.
  - This is a convenience utility to clear the console output for a cleaner user experience.

- **`touch`**: 
  - Creates an empty file if it doesn't already exist or updates the timestamp of an existing file.
  - This system call is useful for managing file metadata.

- **`cpr`**: 
  - Initiates a system-wide checkpointing process.
  - It creates a snapshot of the system’s state, allowing for recovery in case of failure.

- **`chpr`**: 
  - Changes the priority of a process at runtime.
  - This is similar to `nice`, but allows the user to adjust the priority of a process that is already running.

### **4. User Programs**
Developed user-level programs for managing users, groups, and permissions:

#### **`getty`**
- First program executed after boot.  
- Authenticates user credentials using `/passwd` and starts a shell in the user’s home directory.

#### **`passwd [username]`**
- Allows users to change their passwords.  
- Root users can change any password without entering the old one.

#### **`useradd`**
- Adds a new user.  
- Options for setting UID, home directory, and real name.  
- Automatically creates a group with the same name as the username.

#### **`groupadd`**
- Adds a new group to the system.  
- Allows custom GID assignment.

#### **`usermod`**
- Modifies existing user attributes such as username, UID, and group memberships.  

#### **`chmod`**
- Changes file permissions using symbolic or octal modes.  
- Example: Add execute permission to all users with `chmod a+x file`.

### **5. Kernel Modifications**
- Modified the inode structure to include permission fields.  
- Integrated user and group handling into the kernel.  
- Ensured UID/GID validation for file operations.

### **6. Testing and Validation**
- Tested the new features on QEMU with the RISC-V toolchain.  
- Verified user login, file access control, and administrative tasks.

---

## **Build and Run Instructions** ( report on Linkedin) : Pollah Yves 
1. Install the RISC-V toolchain from [RISC-V GNU Toolchain](https://github.com/riscv/riscv-gnu-toolchain).  
2. Install QEMU   
3. Clone the project repository and navigate to the directory.  
4. Build and start XV6:

---

## **Acknowledgments**
This project is based on the XV6 operating system, a reimplementation of Unix Version 6.  
- Inspired by John Lions's *Commentary on UNIX 6th Edition*.  
 

---

## **Error Reports**
For feedback or issues, contact:  
- **Yves Programmeur** yves.pollah@facsciences-uy1.cm
- **Ashley Jonson** ashley.ngnapa@facsciences-uy1.cm
- **Kouega Youmbi** kouega.youmbi@facsciences-uy1.cm


---
