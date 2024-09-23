use reqwest::blocking::Client;
use dotenvy::dotenv;
use std::env;

fn load_env() -> String {
	dotenv().ok();
	env::var("SERVER").expect("SERVER must be set")
}

#[test]
fn test_request_index() {
	let server = load_env();
	let client = Client::new();
	let response = client.get(server).send().unwrap();
	dbg!(&response);
	assert!(response.status().is_success());
}

#[test]
fn test_request_not_found() {
	let server = load_env();
	let client = Client::new();
	let response = client.get(format!("{}/not-found-1234", server)).send().unwrap();
	dbg!(&response);
	assert!(response.status().is_client_error());
}

// #[test]
// fn test_post_index() {
// 	let server = load_env();
// 	let client = Client::new();
// 	let response = client.post(server).send().unwrap();
// 	dbg!(&response);
// 	assert!(response.status().is_success());
// }
