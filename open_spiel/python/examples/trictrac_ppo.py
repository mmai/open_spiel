import pyspiel
import tensorflow.compat.v1 as tf
tf.disable_v2_behavior()

from open_spiel.python import rl_environment
from open_spiel.python.pytorch import ppo
# from open_spiel.python.algorithms import ppo

# ------------------------
# 1. Jeu
# ------------------------

game = pyspiel.load_game("python_trictrac")

env = rl_environment.Environment(
    game,
    observation_type=rl_environment.ObservationType.OBSERVATION
)

num_players = env.num_players

# ------------------------
# 2. Session TF
# ------------------------

sess = tf.Session()

# ------------------------
# 3. Agents PPO
# ------------------------

agents = []
for player_id in range(num_players):
    agent = ppo.PPO(
        session=sess,
        player_id=player_id,
        state_representation_size=env.observation_spec()["info_state"][0],
        num_actions=env.action_spec()["num_actions"],
        hidden_layers_sizes=[256, 256],
        learning_rate=3e-4,
    )
    agents.append(agent)

sess.run(tf.global_variables_initializer())

# ------------------------
# 4. Entraînement
# ------------------------

num_episodes = 100_000

for episode in range(num_episodes):
    time_step = env.reset()

    while not time_step.last():
        player_id = time_step.current_player()

        if player_id >= 0:
            action = agents[player_id].step(time_step).action
        else:
            action = None

        time_step = env.step(action)

    for agent in agents:
        agent.step(time_step)

    if episode % 1000 == 0:
        print(f"Episode {episode}")

# ------------------------
# 5. Sauvegarde
# ------------------------

saver = tf.train.Saver()
saver.save(sess, "ppo_trictrac_model.ckpt")

print("Modèle PPO sauvegardé.")
