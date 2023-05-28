package pl.pg.eti.dp.ClientServerJava.server;

// creating new thread for reading from the pipe
import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;

public class Server {
    private static final int PORT = 12345;
    private List<ClientHandler> activeClientHandlers;

    public Server() {
        this.activeClientHandlers = new CopyOnWriteArrayList<>();
    }

    public void start() throws IOException {
        ServerSocket serverSocket = new ServerSocket(PORT);
        System.out.println("Server listening on port " + PORT);

        ExecutorService executorService = Executors.newCachedThreadPool();

        while (true) {
            Socket clientSocket = serverSocket.accept();
            System.out.println("Client connected: " + clientSocket.getInetAddress().getHostAddress());

            ClientHandler clientHandler = new ClientHandler(clientSocket);
            activeClientHandlers.add(clientHandler);
            executorService.submit(clientHandler);
        }
    }

    private void broadcastMessage(String message, ClientHandler sender) throws IOException {
        for (ClientHandler client : activeClientHandlers) {
            if (client != sender) {
                client.sendMessage(message);
            }
        }
    }

    private class ClientHandler implements Runnable {
        private final Socket clientSocket;
        private final LinkedBlockingQueue<String> messageQueue;

        public ClientHandler(Socket clientSocket) throws IOException {
            this.clientSocket = clientSocket;
            this.messageQueue = new LinkedBlockingQueue<>();
        }

        public void sendMessage(String message) {
            messageQueue.add(message);
        }

        @Override
        public void run() {
            try (
                    BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                    OutputStream outputStream = clientSocket.getOutputStream();
            ) {
                // Create a separate thread to read messages from the pipe
                Thread pipeReaderThread = new Thread(() -> {
                    try {
                        while (true) {
                            String message = messageQueue.take();
                            outputStream.write(message.getBytes(StandardCharsets.UTF_8));
                            outputStream.flush();
                        }
                    } catch (IOException | InterruptedException e) {
                        e.printStackTrace();
                    }
                });

                pipeReaderThread.start();

                String message;
                while ((message = reader.readLine()) != null) {
                    System.out.println("Client sent a message: " + message);
                    broadcastMessage(message, this);
                }
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                try {
                    clientSocket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
