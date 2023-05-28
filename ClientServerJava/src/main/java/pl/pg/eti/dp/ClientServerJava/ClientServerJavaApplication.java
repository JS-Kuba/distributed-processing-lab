package pl.pg.eti.dp.ClientServerJava;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import pl.pg.eti.dp.ClientServerJava.server.Server;

import java.io.IOException;

@SpringBootApplication
public class ClientServerJavaApplication {

	public static void main(String[] args) throws IOException {
		SpringApplication.run(ClientServerJavaApplication.class, args);

		Server server = new Server();
		server.start();
	}

}
