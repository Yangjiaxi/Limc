extern crate limc;

use limc::preprocess::Preprocessor;
use limc::token::{print_token_stream, tokenize};

use std::env;
use std::process;

fn usage() -> ! {
    eprintln!("Usage: 9cc <file>");
    process::exit(1)
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        usage();
    }
    let path = args[1].clone();

    // Tokenize and parse.
    let tokens = tokenize(path, &mut Preprocessor::new());
    print_token_stream(&tokens);
}
