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

#[test]
fn send_two_requests_at_once() {
	let server = load_env();
	let client = Client::new();
	let response1 = client.get(&server).send().unwrap();
	let response2 = client.get(server).send().unwrap();
	dbg!(&response1);
	dbg!(&response2);
	assert!(response1.status().is_success());
	assert!(response2.status().is_success());
}

#[test]
fn send_hundred_requests_at_once() {
	let server = load_env();
	let client = Client::new();
	let mut responses = Vec::new();
	for _ in 0..100 {
		let response = client.get(&server).send().unwrap();
		responses.push(response);
	}
	for response in responses {
		assert!(response.status().is_success());
	}
}

// #[test]
// fn send_chunked_request() {

// }


// #[test]
// fn test_post_index() {
// 	let server = load_env();
// 	let client = Client::new();
// 	let response = client.post(server).send().unwrap();
// 	dbg!(&response);
// 	assert!(response.status().is_success());
// }
