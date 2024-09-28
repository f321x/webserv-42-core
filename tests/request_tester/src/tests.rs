use futures::stream;
use rand::Rng;
use reqwest::{Body, Client};
use dotenvy::dotenv;
use tokio::task::JoinSet;
use std::{convert::Infallible, env, vec};

fn load_env() -> String {
	dotenv().ok();
	env::var("SERVER").expect("SERVER must be set")
}

#[tokio::test]
async fn test_request_index() {
	let server = load_env();
	let client = Client::new();
	let response = client.get(server).send().await.unwrap();
	dbg!(&response);
	assert!(response.status().is_success());
}

#[tokio::test]
async fn test_request_not_found() {
	let server = load_env();
	let client = Client::new();
	let response = client.get(format!("{}/not-found-1234.html", server)).send().await.unwrap();
	dbg!(&response);
	assert!(response.status() == 404);
}

#[tokio::test]
async fn send_two_requests_at_once() {
	let server = load_env();
	let client = Client::new();
	let response1 = client.get(&server).send().await.unwrap();
	let response2 = client.get(server).send().await.unwrap();
	dbg!(&response1);
	dbg!(&response2);
	assert!(response1.status().is_success());
	assert!(response2.status().is_success());
}

#[tokio::test]
async fn send_thousand_requests_after_each_other() {
	let server = load_env();
	let client = Client::new();
	let mut responses = Vec::new();
	for _ in 0..1000 {
		let response = client.get(&server).send().await.unwrap();
		responses.push(response);
	}
	for response in responses {
		assert!(response.status().is_success());
	}
}

#[tokio::test]
async fn send_many_requests_in_parallel() {
	let server = load_env();
	let mut futures = JoinSet::new();
	for _ in 0..500 {
		let server_clone = server.clone();
		futures.spawn(async move {
			let client = Client::new();
			let response = client.get(server_clone).send().await.unwrap();
			assert!(response.status().is_success());
		});
	}
	// await all
	futures.join_all().await;
}

#[tokio::test]
async fn send_chunked_request() {
        let chunked_body = vec!["Hello", ", ", "worl", "d", "!"];
    let stream = stream::iter(chunked_body.into_iter().map(Result::<_, Infallible>::Ok));
    let body = Body::wrap_stream(stream);
        let server = load_env();
        let client = Client::new();
        let response = client.post(server).body(body).send().await.unwrap();
        dbg!(&response);
        assert!(response.status().is_success());
}

#[tokio::test]
async fn send_large_chunked_request_of_random_data() {
	// let mut rng = rand::thread_rng();
	// let chunked_body: Vec<Vec<u8>> = (0..1024)
    // .map(|_| {
    //     let mut buf = vec![0u8; 1024];
    //     rng.fill(&mut buf[..]);
    //     buf
    // })
    // .collect();
	let chunked_body = vec![vec![b'A'; 2048], vec![b'B'; 1024], vec![b'C'; 2048], vec![b'D'; 1024], vec![b'E'; 4096], vec![b'F'; 1024], vec![b'G'; 1024], vec![b'H'; 1024], vec![b'I'; 1024], vec![b'J'; 1024], vec![b'K'; 1024], vec![b'L'; 1024], vec![b'M'; 1024], vec![b'N'; 1024], vec![b'O'; 1024], vec![b'P'; 1024], vec![b'Q'; 1024], vec![b'R'; 1024], vec![b'S'; 1024], vec![b'T'; 1024], vec![b'U'; 1024], vec![b'V'; 1024], vec![b'W'; 1024], vec![b'X'; 1024], vec![b'Y'; 1024], vec![b'Z'; 1024]];
	let stream = stream::iter(chunked_body.into_iter().map(Result::<_, Infallible>::Ok));
	let body = Body::wrap_stream(stream);
	let server = load_env();
	let client = Client::new();
	let response = client.post(server).body(body).send().await.unwrap();
	dbg!(&response);
	assert!(response.status().is_success());
}

// #[test]
// fn test_post_index() {
// 	let server = load_env();
// 	let client = Client::new();
// 	let response = client.post(server).send().unwrap();
// 	dbg!(&response);
// 	assert!(response.status().is_success());
// }
