use std::env;
use std::fs::{OpenOptions};
use std::io::Write;
use std::path::Path;
use std::process::Command;
use structopt::StructOpt;
use termion::color;

#[derive(Debug, StructOpt)]
#[structopt(about = "Add alias command to .my_aliases.zsh file")]
struct Opt {
    #[structopt(short = "n", long = "name", help = "Alias name")]
    alias_name: String,

    #[structopt(short = "c", long = "command", help = "Alias command", required = true)]
    alias_command: Vec<String>,

    #[structopt(short = "f", long = "force", help = "Force create alias without confirmation")]
    force: bool,
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let opt = Opt::from_args();

    // Print out the alias details
    println!(
        "{}Alias Name:{} {}",
        color::Fg(color::Yellow),
        color::Fg(color::Reset),
        opt.alias_name
    );
    println!(
        "{}Alias Command:{} {}",
        color::Fg(color::Yellow),
        color::Fg(color::Reset),
        opt.alias_command.join(" ")
    );

    // Prompt for confirmation, unless the force flag is provided
    if !opt.force {
        println!("Do you want to create this alias? (y/n)");
        let mut input = String::new();
        std::io::stdin().read_line(&mut input)?;
        let confirmation = input.trim().to_lowercase();

        match confirmation.as_str() {
            "y" | "yes" => {
                create_alias(&opt.alias_name, &opt.alias_command)?;
            }
            _ => {
                println!("Alias creation cancelled by user.");
            }
        }
    } else {
        create_alias(&opt.alias_name, &opt.alias_command)?;
    }

    Ok(())
}

// Helper function to create the alias
fn create_alias(alias_name: &str, alias_command: &[String]) -> Result<(), Box<dyn std::error::Error>> {
    // Join alias_command vector into a single string
    let alias_command = alias_command.join(" ");

    // Open .my_aliases.zsh file for appending
    let home_dir = env::var("HOME").expect("Failed to get home directory");
    let alias_file_path = Path::new(&home_dir).join(".my_aliases.zsh");
    let mut file = OpenOptions::new()
        .append(true)
        .create(true)
        .open(&alias_file_path)?;

    // Write alias to .my_aliases.zsh file
    writeln!(&mut file, "alias {}='{}'", alias_name, alias_command)?;

    println!(
        "{}Alias '{}' with command '{}' has been added to .my_aliases.zsh{}",
        color::Fg(color::Green),
        alias_name,
        alias_command,
        color::Fg(color::Reset)
    );

    // Reload the .my_aliases.zsh file in the same shell
    Command::new("zsh")
        .arg("-i")
        .spawn()?
        .wait()?;
    Ok(())
}
