<?php

/**
 * @file
 * Theme and preprocess functions for menus.
 */

declare(strict_types = 1);

use Drupal\Core\Template\Attribute;

/**
 * Implements hook_preprocess_menu().
 */
function oe_bootstrap_theme_preprocess_menu(&$variables, $hook) {
  if ($hook === 'menu__toolbar') {
    return;
  }

  $variables['pattern_items'] = [];
  foreach ($variables['items'] as $key => $item) {
    $pattern_item = [
      'label' => $item['title'],
      'path' => $item['url']->toString(),
      'attributes' => new Attribute([
        'class' => $item['in_active_trail'] ? ['active'] : [],
      ]),
    ];
    if (empty($item['below'])) {
      $variables['pattern_items'][$key] = $pattern_item;
      continue;
    }

    $submenu = $pattern_item;
    $submenu['attributes'] = new Attribute(['class' => 'nav-link']);
    $pattern_item += [
      'standalone' => TRUE,
      'dropdown' => TRUE,
      'link' => TRUE,
      'alignment' => 'dropdown-menu_item-sm-start',
      'trigger' => $submenu,
    ];
    foreach ($item['below'] as $key_sub_item => $sub_item) {
      $pattern_item['items'][$key_sub_item] = [
        'label' => $sub_item['title'],
        'path' => $sub_item['url']->toString(),
        'attributes' => new Attribute([
          'class' => $sub_item['in_active_trail'] ? ['active'] : [],
        ]),
      ];
    }
    $variables['pattern_items'][$key] = $pattern_item;
  }

}

/**
 * Implements template_preprocess_menu_local_action().
 */
function oe_bootstrap_theme_preprocess_menu_local_action(&$variables) {
  // Add button classes.
  $variables['link']['#options']['attributes']['class'][] = 'btn';
  $variables['link']['#options']['attributes']['class'][] = 'btn-sm';
  $variables['link']['#options']['attributes']['class'][] = 'btn-primary';
}

/**
 * Implements template_preprocess_links__dropbutton().
 */
function oe_bootstrap_theme_preprocess_links__dropbutton(&$variables) {
  $links = &$variables['links'];

  // Do nothing if we have no links.
  if (!count($links)) {
    return;
  }

  // Get the first link and use it for the dropbutton.
  $link = reset($links);

  /** @var \Drupal\Core\Url $url */
  $variables['split'] = FALSE;
  $button = $link['text'];
  if (isset($link['link']) && ($url = $link['link']['#url'])) {
    $button = $link['link'];

    if ($url->getRouteName() !== '<nolink>') {
      $variables['split'] = TRUE;
      $button['#options']['attributes']['class'][] = 'btn';
      $button['#options']['attributes']['class'][] = 'btn-sm';
      $button['#options']['attributes']['class'][] = 'btn-outline-primary';
    }

    // Remove first link from links.
    array_shift($links);
  }

  $variables['button'] = $button;
  oe_bootstrap_theme_add_dropbutton_classes($links);
}

/**
 * Adds required classes to dropbutton links.
 *
 * @param array $links
 *   Array with links items.
 */
function oe_bootstrap_theme_add_dropbutton_classes(array &$links): void {
  foreach ($links as $key => $link) {
    if (isset($links[$key]['link']) && is_array($links[$key]['link'])) {
      $links[$key]['link']['#options']['attributes']['class'][] = 'dropdown-item';
    }
    if (isset($links[$key]['text_attributes'])) {
      $links[$key]['text_attributes']->addClass('dropdown-item');
    }
    if (isset($links[$key]['attributes'])) {
      $links[$key]['attributes']->addClass('dropdown-item');
    }
    if (isset($links[$key]['text']) && is_array($links[$key]['text'])) {
      $links[$key]['text']['#attributes']['class'][] = 'dropdown-item';
    }
  }
}

/**
 * Implements hook_preprocess_menu_local_tasks().
 */
function oe_bootstrap_theme_preprocess_menu_local_tasks(array &$variables): void {

  // Sort local tasks by weight.
  uasort($variables['primary'], [
    '\Drupal\Component\Utility\SortArray',
    'sortByWeightProperty',
  ]);
  uasort($variables['secondary'], [
    '\Drupal\Component\Utility\SortArray',
    'sortByWeightProperty',
  ]);

  foreach ($variables['primary'] as $link) {
    oe_bootstrap_theme_prepare_menu_local_task_links($link, $variables, 'primary');
  }

  foreach ($variables['secondary'] as $link) {
    oe_bootstrap_theme_prepare_menu_local_task_links($link, $variables, 'secondary');
  }
}

/**
 * Prepare menu local tasks render links.
 *
 * @param array $link
 *   Array with the links items to be prepared.
 * @param array $variables
 *   The render array.
 * @param string $type
 *   Discriminate between primary or secondary.
 */
function oe_bootstrap_theme_prepare_menu_local_task_links(array $link, array &$variables, string $type): void {
  /** @var Drupal\Core\Url $url */
  $url = $link['#link']['url'];
  if ($url->access($variables['user'])) {
    $variables[$type . '_links'][] = [
      'link_style' => $type,
      'path' => $url->toString(),
      'label' => $link['#link']['title'],
      'standalone' => TRUE,
      'attributes' => $link['#active'] ? new Attribute(['class' => 'active']) : [],
    ];
  }
}
