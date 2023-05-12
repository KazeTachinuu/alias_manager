use std::env;
use std::fs::File;
use std::fs::OpenOptions;
use std::io::{self, BufRead, BufReader, BufWriter, Write};
use std::process::Command;
use structopt::StructOpt;
use termion::color;

#[derive(Debug, StructOpt)]
#[structopt(about = "Manage aliases in .my_aliases.txt file")]
enum Opt {
    #[structopt(name = "add", about = "Add an alias")]
    Add {
        #[structopt(short = "n", long = "name", help = "Alias name")]
        alias_name: String,

        #[structopt(short = "c", long = "command", help = "Alias command", required = true)]
        alias_command: Vec<String>,
    },
    #[structopt(name = "rm", about = "Remove an alias")]
    Remove {
        #[structopt(short = "n", long = "name", help = "Alias name")]
        alias_name: String,
    },
    #[structopt(name = "list", about = "List all aliases")]
    List,
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let opt = Opt::from_args();

    match opt {
        Opt::Add {
            alias_name,
            alias_command,
        } => {
            // Print out the alias details
            println!(
                "{}Alias Name:{} {}",
                color::Fg(color::Yellow),
                color::Fg(color::Reset),
                alias_name
            );
            println!(
                "{}Alias Command:{} {}",
                color::Fg(color::Yellow),
                color::Fg(color::Reset),
                alias_command.join(" ")
            );

            // Prompt for confirmation
            println!("Do you want to create this alias? (Y/n)");
            let mut input = String::new();
            std::io::stdin().read_line(&mut input)?;
            let confirmation = input.trim().to_lowercase();

            match confirmation.as_str() {
                "y" | "yes" | "" => {
                    create_alias(&alias_name, &alias_command)?;
                }
                _ => {
                    println!("Alias creation cancelled by user.");
                }
            }
        }
        Opt::Remove { alias_name } => {
            remove_alias(&alias_name, false)?;
        }
        Opt::List => {
            list_aliases()?;
        }
    }

    // [TODO]: source alias file without keeping the program running in the background after it exits
    /* let home = env::var("HOME")?;
    let file_path = format!("{}/.my_aliases.txt", home);
    let cmd = "exec ".to_string() + &file_path;
        println!("{}", cmd);
    Command::new(cmd);
    */
    Command::new("exec zsh");

    //terminate the program
    std::process::exit(0);
}

/// Helper function to create an alias
fn create_alias(
    alias_name: &str,
    alias_command: &[String],
) -> Result<(), Box<dyn std::error::Error>> {
    // Join alias_command vector into a single string
    let alias_command = alias_command.join(" ");

    // Open .my_aliases.txt file for reading
    let home = env::var("HOME")?;
    let file_path = format!("{}/.my_aliases.txt", home);
    let file_content = std::fs::read_to_string(&file_path)?;

    // Check if alias already exists
    if file_content.contains(&alias_name) {
        println!(
            "Alias '{}' already exists. Do you want to update it with the new command? (Y/n)",
            alias_name
        );
        let mut input = String::new();
        std::io::stdin().read_line(&mut input)?;
        let confirmation = input.trim().to_lowercase();
        if confirmation == "y" || confirmation == "yes" || confirmation == "" {
            //remove the old alias
            remove_alias(&alias_name, true)?;
        } else {
            println!("Alias creation cancelled by user.");
            return Ok(());
        }
    }

    // Open .my_aliases.txt file for appending
    let mut file = OpenOptions::new()
        .append(true)
        .create(true)
        .open(&file_path)?;

    // Write alias to the file
    writeln!(file, "alias {}='{}'", alias_name, alias_command)?;

    println!(
        "Alias '{}' with command '{}' created successfully!\nReload the terminal to use it.",
        alias_name, alias_command
    );

    Ok(())
}

fn remove_alias(alias_name: &str, is_forced: bool) -> io::Result<()> {
    // Open .my_aliases.txt file for reading
    let home = env::var("HOME").map_err(|err| {
        io::Error::new(
            io::ErrorKind::Other,
            format!("Failed to get HOME directory: {}", err),
        )
    })?;
    let file_path = format!("{}/.my_aliases.txt", home);
    let file = File::open(&file_path)?;

    // Read the lines from the file into a vector
    let lines: Vec<String> = BufReader::new(file)
        .lines()
        .map(|line| line.expect("Failed to read line"))
        .collect();

    if !is_forced {
        // Display the alias to be removed
        println!("Removing alias:");
        for line in &lines {
            if line.starts_with(&format!("alias {}=", alias_name)) {
                println!("{}", line);
                break;
            }
        }
        // Prompt user for confirmation
        println!("Do you want to remove this alias? (Y/n)");
        let mut confirmation = String::new();
        io::stdin().read_line(&mut confirmation)?;
        let confirmation = confirmation.trim().to_lowercase();
        if confirmation != "y" && confirmation != "yes" && confirmation != "" {
            println!("Aborted.");
            return Ok(());
        }
    }

    // Open the file again for writing
    let file = File::create(&file_path)?;
    let mut writer = BufWriter::new(file);

    // Write the lines back to the file, excluding the one containing the alias
    let alias_line = format!("alias {}=", alias_name);
    for line in lines {
        if !line.starts_with(&alias_line) {
            writeln!(writer, "{}", line)?;
        }
    }

    writer.flush()?;
    if !is_forced {
        println!("Alias '{}' removed successfully!", alias_name);
    }
    Ok(())
}

// Helper function to list all aliases
fn list_aliases() -> Result<(), Box<dyn std::error::Error>> {
    // Open .my_aliases.txt file for reading
    let home = env::var("HOME")?;
    let file_path = format!("{}/.my_aliases.txt", home);
    let file_content = std::fs::read_to_string(&file_path)?;

    // Extract aliases from the file content
    let aliases: Vec<&str> = file_content
        .lines()
        .filter(|line| line.starts_with("alias"))
        .collect();

    println!(
        "{}Aliases:{}",
        color::Fg(color::Green),
        color::Fg(color::Reset)
    );
    for alias in aliases {
        let alias = alias.replace("alias ", "");
        let alias = alias.split('=').collect::<Vec<&str>>();
        println!(
            "{}{}{} => {}{}{}",
            color::Fg(color::LightBlue),
            alias[0],
            color::Fg(color::Reset),
            color::Fg(color::Yellow),
            alias[1],
            color::Fg(color::Reset)
        );
    }

    Ok(())
}
